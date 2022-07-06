#include <Precomp.h>
#include "SceneRenderer.h"

#include <Opium/Application.h>

#include <Renderer/VertexArray.h>
#include <Renderer/Shader.h>
#include <Renderer/UniformBuffer.h>
#include <Renderer/RenderCommand.h>
#include <Renderer/ShaderPass.h>

#include <Renderer/Texture.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Temp

#include <glad/glad.h>

#include <Geometry/Icosphere.h>
#include <Geometry/Cube.h>
#include <Geometry/Plane.h>

namespace OP
{

	struct SceneRendererData
	{
		// Shaders
		Ref<Shader> mainShader;
		Ref<Shader> depthShader;
		Ref<Shader> depthDebugShader;

		// Ref<Texture2D> woodTexture;
		Ref<Texture2D> WhiteTexture;

		Ref<Icosphere> icosphere;
		Ref<Icosphere> icosphere2;
		Ref<Icosphere> icosphere3;
		Ref<Cube> cube;
		Ref<Plane> plane;

		// Ref<Icosphere> icosphere2;


		// Directional light
		glm::vec3 lightPos{-2.0f, 10.0f, -1.0f};


		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec3 ViewPos;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		struct ModelLightSpaceData
		{
			glm::mat4 Model;
			glm::mat4 LightSpaceMatrix;

			// Test
			glm::vec3 Color;
		};

		// Temp
		struct LightData
		{
			glm::vec3 LightPos;
		};

		ModelLightSpaceData ModelLightSpaceBuffer;
		Ref<UniformBuffer> ModelLightSpaceUniformBuffer;

		LightData LightBuffer;
		Ref<UniformBuffer> LightUniformBuffer;

		glm::vec2 ViewportSize{0.0f, 0.0f};


		// Framebuffer
		Ref<Framebuffer> depthFramebuffer;
		Ref<Framebuffer> finalFramebuffer;


		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;

		Ref<ShaderPass> depthShaderPass;
		Ref<ShaderPass> finalShaderPass;

		// Temp
		glm::mat4 spinningModel;

	};


	static SceneRendererData s_SceneRendererData;

	void SceneRenderer::Init(float width, float height, Ref<Framebuffer> fB)
	{

		s_SceneRendererData.spinningModel = glm::mat4(1.0f);
		s_SceneRendererData.spinningModel = glm::translate(s_SceneRendererData.spinningModel, glm::vec3(-4.0f, 6.0f, 5.0f));
		s_SceneRendererData.spinningModel = glm::scale(s_SceneRendererData.spinningModel, glm::vec3(1.0f, 1.0f, 1.0f));

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		s_SceneRendererData.icosphere = Icosphere::Create(1.0f, 2, true);
		s_SceneRendererData.icosphere2 = Icosphere::Create(2.0f, 0, false);
		s_SceneRendererData.icosphere3 = Icosphere::Create(1.5f, 1, false);
		s_SceneRendererData.cube = Cube::Create();
		s_SceneRendererData.plane = Plane::Create();

		s_SceneRendererData.mainShader = Shader::Create("assets/shaders/Pbr/Main.glsl");
		s_SceneRendererData.depthShader = Shader::Create("assets/shaders/Pbr/Depth.glsl");
		s_SceneRendererData.depthDebugShader = Shader::Create("assets/shaders/Pbr/DepthDebug.glsl");

		s_SceneRendererData.CameraUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::CameraData), 0);
		s_SceneRendererData.ModelLightSpaceUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::ModelLightSpaceData), 1);
		s_SceneRendererData.LightUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::LightData), 2);

		s_SceneRendererData.ViewportSize.x = width;
		s_SceneRendererData.ViewportSize.y = height;

		// Create white texture
		s_SceneRendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_SceneRendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// Arrange Framebuffers

		// Depth framebuffer
		FramebufferSpecification depthFBSpec;
		depthFBSpec.Attachments = { FramebufferTextureFormat::DEPTH16 };
		depthFBSpec.Width = s_SceneRendererData.ViewportSize.x;
		depthFBSpec.Height = s_SceneRendererData.ViewportSize.y;
		s_SceneRendererData.depthFramebuffer = Framebuffer::Create(depthFBSpec);

		s_SceneRendererData.depthShaderPass = ShaderPass::Create(std::string("Depth Pass"), depthFBSpec, s_SceneRendererData.depthShader);

		// Final Framebuffer
		
		FramebufferSpecification finalFBSpec;
		finalFBSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		finalFBSpec.Width = s_SceneRendererData.ViewportSize.x;
		finalFBSpec.Height = s_SceneRendererData.ViewportSize.y;
		s_SceneRendererData.finalFramebuffer = Framebuffer::Create(finalFBSpec);

		s_SceneRendererData.finalShaderPass = ShaderPass::Create(std::string("Final Pass"), fB);



		s_SceneRendererData.finalFramebuffer = fB;

		
		// Configure Shaders
		s_SceneRendererData.mainShader->Bind();
		s_SceneRendererData.mainShader->SetInt("u_DiffuseTexture", 0);
		s_SceneRendererData.mainShader->SetInt("u_ShadowMap", 0);

		s_SceneRendererData.depthDebugShader->Bind();
		s_SceneRendererData.depthDebugShader->SetInt("u_DepthMap", 0);


	}

	void SceneRenderer::ResizeViewport(float width, float height)
	{
		if (s_SceneRendererData.ViewportSize.x != width || s_SceneRendererData.ViewportSize.y != height)
		{
			// Update viewport size
			s_SceneRendererData.ViewportSize.x = width;
			s_SceneRendererData.ViewportSize.y = height;

			// Resize necessary framebuffers
			s_SceneRendererData.depthFramebuffer->Resize(width, height);
			s_SceneRendererData.finalFramebuffer->Resize(width, height);

			s_SceneRendererData.depthShaderPass->ResizeFramebuffer(width, height);
		}
	}

	void SceneRenderer::Render(EditorCamera& camera, Timestep ts)
	{

		float time = (float)Application::Get().GetWindow().GetTime();

		s_SceneRendererData.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_SceneRendererData.CameraBuffer.ViewPos = camera.GetPosition();
		s_SceneRendererData.CameraUniformBuffer->SetData(&s_SceneRendererData.CameraBuffer, sizeof(SceneRendererData::CameraData));

		// Render depth of scene to texture (from light's perspective)
		// -----------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 0.2f, far_plane = 505.0f;
		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		s_SceneRendererData.lightPos = glm::vec3(-2 + sin(glm::radians(time)), 10.0f, -1 + cos(glm::radians(time)));
		lightView = glm::lookAt(s_SceneRendererData.lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		// render scene from light's point of view
		//s_SceneRendererData.depthShader->Bind();
		//s_SceneRendererData.ModelLightSpaceBuffer.LightSpaceMatrix = lightSpaceMatrix;
		//s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
		//glViewport(0, 0, s_SceneRendererData.ViewportSize.x, s_SceneRendererData.ViewportSize.y);

		s_SceneRendererData.LightBuffer.LightPos = s_SceneRendererData.lightPos;
		s_SceneRendererData.LightUniformBuffer->SetData(&s_SceneRendererData.LightBuffer, sizeof(SceneRendererData::LightData));

		glm::mat4 model = glm::mat4(1.0f);

		


		s_SceneRendererData.depthShaderPass->InvokeCommands(
			[&] () -> void {

				s_SceneRendererData.depthShader->Bind();
				s_SceneRendererData.ModelLightSpaceBuffer.LightSpaceMatrix = lightSpaceMatrix;
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				glViewport(0, 0, s_SceneRendererData.ViewportSize.x, s_SceneRendererData.ViewportSize.y);

				glClear(GL_DEPTH_BUFFER_BIT);
				glCullFace(GL_FRONT);
				// ------------ DRAW SCENE ------------


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
				s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
				// yellowish
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.7f, 0.6f, 0.2f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.icosphere->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
				model = glm::scale(model, glm::vec3(60.0f, 0.5f, 60.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
				s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
				// bright grey
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.9f, 0.9f, 0.9f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 2.0f));
				model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
				s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
				// dark orange
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.8f, 0.2f, 0.4f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.cube->Draw();


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(5.0f, 3.0f, -2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
				// dark orange
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.1f, 0.8f, 0.4f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.icosphere2->Draw();



				s_SceneRendererData.spinningModel = glm::rotate(s_SceneRendererData.spinningModel, glm::radians((float)ts * time * 15.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
				s_SceneRendererData.ModelLightSpaceBuffer.Model = s_SceneRendererData.spinningModel;
				// dark orange
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.1f, 0.5f, 0.9f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.icosphere3->Draw();


				// ---------- DRAW SCENE END ----------
				glCullFace(GL_BACK);
			}
		);


		// FINAL RENDERING - (FOR NOW!)

		s_SceneRendererData.finalShaderPass->InvokeCommands(
			[&]()-> void {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				s_SceneRendererData.mainShader->Bind();

				s_SceneRendererData.WhiteTexture->Bind(0);

				uint32_t depthMap = ShaderPass::GetDepthInput(s_SceneRendererData.depthShaderPass);
				glBindTextureUnit(1, depthMap);
				
			
				// ------------ DRAW SCENE ------------


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
				s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
				// yellowish
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.7f, 0.6f, 0.2f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.icosphere->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
				model = glm::scale(model, glm::vec3(60.0f, 0.5f, 60.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
				s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
				// bright grey
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.9f, 0.9f, 0.9f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 2.0f));
				model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
				s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
				// dark orange
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.8f, 0.2f, 0.4f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(5.0f, 3.0f, -2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
				// dark orange
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.1f, 0.8f, 0.4f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.icosphere2->Draw();


				s_SceneRendererData.spinningModel = glm::rotate(s_SceneRendererData.spinningModel, glm::radians((float)ts * time * 15.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
				s_SceneRendererData.ModelLightSpaceBuffer.Model = s_SceneRendererData.spinningModel;
				// dark orange
				s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.1f, 0.5f, 0.9f);
				s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
				s_SceneRendererData.icosphere3->Draw();
				// ---------- DRAW SCENE END ----------
			}
		);

		/*s_SceneRendererData.finalFramebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		s_SceneRendererData.mainShader->Bind();



		//simdilik kapa glBindTexture(GL_TEXTURE_2D, s_SceneRendererData.depthFramebuffer->GetColorAttachmentRendererID(0));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
		s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
		// yellowish
		s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.7f, 0.6f, 0.2f);
		s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
		s_SceneRendererData.icosphere->Draw();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
		s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
		// bright grey
		s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.9f, 0.9f, 0.9f);
		s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
		s_SceneRendererData.plane->Draw();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		s_SceneRendererData.ModelLightSpaceBuffer.Model = model;
		// dark orange
		s_SceneRendererData.ModelLightSpaceBuffer.Color = glm::vec3(0.8f, 0.2f, 0.4f);
		s_SceneRendererData.ModelLightSpaceUniformBuffer->SetData(&s_SceneRendererData.ModelLightSpaceBuffer, sizeof(SceneRendererData::ModelLightSpaceData));
		s_SceneRendererData.cube->Draw(); */

		

	}

	SceneRendererStats SceneRenderer::GetStats()
	{
		return SceneRendererStats();
	}

}