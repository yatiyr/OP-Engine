#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Renderer/Texture.h>
#include <Renderer/UniformBuffer.h>
#include <Renderer/RenderPass.h>
#include <Renderer/Shader.h>
#include <Renderer/Framebuffer.h>

#include <Geometry/Skybox.h>
#include <Geometry/Plane.h>

#include <Renderer/RenderCommand.h>

namespace OP
{

	struct CubemapCaptureViewData
	{
		glm::mat4 CaptureViews[6] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
	};

	struct EnvironmentMapSpec
	{
		Ref<Texture> EquirectangularTex;
		Ref<Shader> CubemapCaptureShader;
		Ref<Shader> IrradianceMapGenerationShader;
		Ref<Shader> PrefilterGenerationShader;
		Ref<Shader> BrdfLUTGenerationShader;
	};

	class EnvironmentMap
	{
	public:

		EnvironmentMap(EnvironmentMapSpec spec);

		void GenerateMaps();

		void GenerateEnvironmentCubemap();
		void GenerateIrradianceMap();
		void GeneratePrefilterMap();
		void GenerateBrdfLUT();

		void BindEnvironmentCubemap(uint32_t slot);
		void BindIrradianceMap(uint32_t slot);
		void BindPrefilterMap(uint32_t slot);
		void BindBrdfLUT(uint32_t slot);

		static Ref<EnvironmentMap> Create(EnvironmentMapSpec spec);
	private:
		Ref<Texture> m_EquirectangularTex;

		Ref<UniformBuffer> m_CubemapCaptureUniformBuffer;

		Ref<RenderPass> m_CubemapCaptureRenderPass;
		Ref<RenderPass> m_IrradianceMapGenerationRenderPass;
		Ref<RenderPass> m_PrefilterGenerationRenderPass;
		Ref<RenderPass> m_BrdfLUTGenerationPass;

		Ref<Shader> m_CubemapCaptureShader;
		Ref<Shader> m_IrradianceMapGenerationShader;
		Ref<Shader> m_PrefilterGenerationShader;
		Ref<Shader> m_BrdfLUTGenerationShader;

		CubemapCaptureViewData m_CubemapCaptureBuffer;

		Ref<Skybox> m_Skybox;
		Ref<Plane> m_Plane;

		glm::mat4 m_CaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	};
}