#include <Precomp.h>
#include <Renderer/EnvironmentMap.h>

// TEMP : There will be no opengl functions left in the future in higher end api
#include <glad/glad.h>

namespace OP
{
	EnvironmentMap::EnvironmentMap(EnvironmentMapSpec spec) :
		m_Name(spec.Name),
		m_EquirectangularTex(spec.EquirectangularTex),
		m_CubemapCaptureShader(spec.CubemapCaptureShader),
		m_IrradianceMapGenerationShader(spec.IrradianceMapGenerationShader),
		m_PrefilterGenerationShader(spec.PrefilterGenerationShader),
		m_BrdfLUTGenerationShader(spec.BrdfLUTGenerationShader),
		m_SkyboxShader(spec.SkyboxShader)
	{
		
		// Create UniformBuffer and Set Its Data
		m_CubemapCaptureUniformBuffer = UniformBuffer::Create(sizeof(CubemapCaptureViewData), 0);
		m_CubemapCaptureUniformBuffer->SetData(&m_CubemapCaptureBuffer, sizeof(CubemapCaptureViewData));

		// Initialize Geometry for Rendering
		m_Skybox = Skybox::Create();
		m_Plane = Plane::Create();

		//GenerateMaps();

	}

	void EnvironmentMap::GenerateMaps()
	{
		RenderCommand::Disable(MODE::DEPTH_TEST);
		RenderCommand::Disable(MODE::CULL_FACE);
		FreeMemory();
		GenerateRenderPasses();
		GenerateEnvironmentCubemap();
		GenerateIrradianceMap();
		GeneratePrefilterMap();
		GenerateBrdfLUT();
		RenderCommand::Enable(MODE::DEPTH_TEST);
		RenderCommand::Enable(MODE::CULL_FACE);
	}

	void EnvironmentMap::GenerateEnvironmentCubemap()
	{
		m_CubemapCaptureRenderPass->InvokeCommands(
			[&]() -> void
			{
				m_CubemapCaptureShader->Bind();
				glBindTextureUnit(0, m_EquirectangularTex->GetRendererID());
				m_CubemapCaptureShader->SetMat4(0, m_CaptureProjection);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				m_Skybox->Draw();
			}
		);

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapCaptureRenderPass->GetColorAttachment(0));
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void EnvironmentMap::GenerateIrradianceMap()
	{
		m_IrradianceMapGenerationRenderPass->InvokeCommands(
			[&]() -> void
			{
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				m_IrradianceMapGenerationShader->Bind();
				uint32_t environmentMap = m_CubemapCaptureRenderPass->GetColorAttachment(0);
				glBindTextureUnit(0, environmentMap);
				m_IrradianceMapGenerationShader->SetMat4(0, m_CaptureProjection);
				m_Skybox->Draw();
			}
		);
	}

	void EnvironmentMap::GeneratePrefilterMap()
	{
		m_PrefilterGenerationRenderPass->InvokeCommands(
			[&]() -> void
			{
				m_PrefilterGenerationShader->Bind();
				uint32_t environmentMap = m_CubemapCaptureRenderPass->GetColorAttachment(0);
				glBindTextureUnit(0, environmentMap);

				float width, height;
				m_PrefilterGenerationRenderPass->GetFramebuffer()->GetSize(width, height);

				m_PrefilterGenerationShader->SetMat4(0, m_CaptureProjection);
				uint32_t maxMipLevels = 5;
				for (uint32_t mip = 0; mip < maxMipLevels; mip++)
				{
					uint32_t mipWidth = static_cast<unsigned int> (width  * std::pow(0.5, mip));
					uint32_t mipHeight = static_cast<unsigned int>(height * std::pow(0.5, mip));
					m_PrefilterGenerationRenderPass->GetFramebuffer()->Resize(mipWidth, mipHeight);
					glViewport(0, 0, mipWidth, mipHeight);

					float roughness = (float)mip / (float)(maxMipLevels - 1);
					m_PrefilterGenerationShader->SetFloat(1, roughness);
					for (uint32_t i = 0; i < 6; i++)
					{
						glm::mat4 view = m_CubemapCaptureBuffer.CaptureViews[i];
						m_PrefilterGenerationShader->SetMat4(2, view);
						uint32_t tex = m_PrefilterGenerationRenderPass->GetFramebuffer()->GetColorAttachmentRendererID(0);
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
							tex, mip);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						m_Skybox->Draw();
					}
				}
			}
		);
	}

	void EnvironmentMap::GenerateBrdfLUT()
	{
		m_BrdfLUTGenerationPass->InvokeCommands(
			[&]() -> void
			{
				m_BrdfLUTGenerationShader->Bind();
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				RenderCommand::Disable(MODE::DEPTH_TEST);
				m_Plane->Draw();
				RenderCommand::Enable(MODE::DEPTH_TEST);
			}
		);
	}

	void EnvironmentMap::BindEnvironmentCubemap(uint32_t slot)
	{
		uint32_t environmentMap = m_CubemapCaptureRenderPass->GetColorAttachment(0);
		glBindTextureUnit(slot, environmentMap);
	}

	void EnvironmentMap::BindIrradianceMap(uint32_t slot)
	{
		uint32_t irradianceMap = m_IrradianceMapGenerationRenderPass->GetColorAttachment(0);
		glBindTextureUnit(slot, irradianceMap);
	}

	void EnvironmentMap::BindPrefilterMap(uint32_t slot)
	{
		uint32_t prefilterMap = m_PrefilterGenerationRenderPass->GetColorAttachment(0);
		glBindTextureUnit(slot, prefilterMap);
	}

	void EnvironmentMap::BindBrdfLUT(uint32_t slot)
	{
		uint32_t brdfLUT = m_BrdfLUTGenerationPass->GetColorAttachment(0);
		glBindTextureUnit(slot, brdfLUT);
	}

	void EnvironmentMap::RenderSkybox()
	{
		RenderCommand::DepthFunc(DEPTHFUNC::LEQUAL);
		glCullFace(GL_FRONT);
		m_SkyboxShader->Bind();
		BindEnvironmentCubemap(0);
		m_Skybox->Draw();
		glCullFace(GL_BACK);
		RenderCommand::DepthFunc(DEPTHFUNC::LESS);
	}

	std::string EnvironmentMap::GetName()
	{
		return m_Name;
	}

	Ref<EnvironmentMap> EnvironmentMap::Create(EnvironmentMapSpec spec)
	{
		return std::make_shared<EnvironmentMap>(spec);
	}

	void EnvironmentMap::FreeMemory()
	{
		if (m_CubemapCaptureRenderPass.get())
		{
			m_CubemapCaptureRenderPass->FreeFramebuffer();
			m_CubemapCaptureRenderPass.reset();
		}
		if (m_IrradianceMapGenerationRenderPass.get())
		{
			m_IrradianceMapGenerationRenderPass->FreeFramebuffer();
			m_IrradianceMapGenerationRenderPass.reset();
		}
		if (m_PrefilterGenerationRenderPass.get())
		{
			m_PrefilterGenerationRenderPass->FreeFramebuffer();
			m_PrefilterGenerationRenderPass.reset();
		}
		if (m_BrdfLUTGenerationPass.get())
		{
			m_BrdfLUTGenerationPass->FreeFramebuffer();
			m_BrdfLUTGenerationPass.reset();
		}
			
	}

	void EnvironmentMap::GenerateRenderPasses()
	{
		// ------------------ CUBEMAP CAPTURE RENDER PASS ---------------- //
		FramebufferSpecification cubemapCFSpec;
		cubemapCFSpec.Attachments = { FramebufferTextureFormat::CUBEMAP_MIP, FramebufferTextureFormat::CUBEMAP_DEPTH };
		cubemapCFSpec.Width = 4096;
		cubemapCFSpec.Height = 4096;

		m_CubemapCaptureRenderPass = RenderPass::Create(std::string("Cubemap Capture Pass"),
			cubemapCFSpec,
			m_CubemapCaptureShader);
		// -------------- IRRADIANCE MAP GENERATION PASS ------------------ //
		FramebufferSpecification irradianceCFSpec;
		irradianceCFSpec.Attachments = { FramebufferTextureFormat::CUBEMAP, FramebufferTextureFormat::CUBEMAP_DEPTH };
		irradianceCFSpec.Width = 32;
		irradianceCFSpec.Height = 32;

		m_IrradianceMapGenerationRenderPass = RenderPass::Create(std::string("Irradiance Map Generation Pass"),
			irradianceCFSpec,
			m_IrradianceMapGenerationShader);
		// -------------- PREFILTER GENERATION PASS ------------------------ //
		FramebufferSpecification prefilterFSpec;
		prefilterFSpec.Attachments = { FramebufferTextureFormat::CUBEMAP_MIP, FramebufferTextureFormat::CUBEMAP_DEPTH_RBO };
		prefilterFSpec.Width = 128;
		prefilterFSpec.Height = 128;

		m_PrefilterGenerationRenderPass = RenderPass::Create(std::string("Prefilter Generation Pass"),
			prefilterFSpec,
			m_PrefilterGenerationShader);
		// -------------- BRDF LUT GENERATION PASS -------------------------- //
		FramebufferSpecification brdfLUTFSpec;
		brdfLUTFSpec.Attachments = { FramebufferTextureFormat::RGBA32F, FramebufferTextureFormat::Depth };
		brdfLUTFSpec.Width = 512;
		brdfLUTFSpec.Height = 512;

		m_BrdfLUTGenerationPass = RenderPass::Create(std::string("BRDF LUT Generation Pass"),
			brdfLUTFSpec,
			m_BrdfLUTGenerationShader);
	}

}
