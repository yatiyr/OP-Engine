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


#define MAX_DIR_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

namespace OP
{

	struct SceneRendererData
	{
		// --------- RENDERER CONSTANTS -------- //
		float Epsilon = 0.00000001;

		// ----------- UNIFORM BUFFERS --------- //

		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec3 ViewPos;
		} CameraBuffer;

		Ref<UniformBuffer> CameraUniformBuffer;

		struct TransformData
		{
			glm::mat4 Model;
		} TransformBuffer;

		Ref<UniformBuffer> TransformUniformBuffer;


		// Needs an overhaul

		// --------- DIRECTIONAL LIGHT ------------- //
		struct DirLight
		{
			glm::mat4 LightSpaceMatrix;
			glm::vec3 LightDir;
			glm::vec3 Color;
		};

		struct DirLightData
		{
			int Size;
			DirLight DirLights[MAX_DIR_LIGHTS];
		} DirLightsBuffer;

		Ref<UniformBuffer> DirLightUniformBuffer;
		// ------- END DIRECTIONAL LIGHT --------- //


		// ----------- SPOT LIGHT --------------- //
		struct SpotLight
		{
			glm::mat4 LightSpaceMatrix;
			glm::vec3 LightDir;
			glm::vec3 Color;
			float Phi;
			float Theta;
		};

		struct SpotLightData
		{
			int Size;
			SpotLight SpotLights[MAX_SPOT_LIGHTS];
		};

		Ref<UniformBuffer> SpotLightUniformBuffer;
		// --------- END SPOT LIGHT ------------ //






		struct LightData
		{
			glm::mat4 LightSpaceMatrix;
			glm::vec3 LightDir;
		} LightBuffer;

		Ref<UniformBuffer> LightUniformBuffer;

		// Needs an overhaul
		struct MaterialData
		{
			glm::vec3 Color;
		} MaterialBuffer;
		
		Ref<UniformBuffer> MaterialUniformBuffer;




		// ------------ OLD PART --------------- //

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

		// ---------- TEMP ----------
		s_SceneRendererData.spinningModel = glm::mat4(1.0f);
		s_SceneRendererData.spinningModel = glm::translate(s_SceneRendererData.spinningModel, glm::vec3(-4.0f, 6.0f, 5.0f));
		s_SceneRendererData.spinningModel = glm::scale(s_SceneRendererData.spinningModel, glm::vec3(1.0f, 1.0f, 1.0f));
		s_SceneRendererData.LightBuffer.LightDir = glm::vec3(0.0f, -1.0f, 0.0f);
		// ---------------

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


		// Deal with uniform buffers
		s_SceneRendererData.CameraUniformBuffer    = UniformBuffer::Create(sizeof(SceneRendererData::CameraData), 0);
		s_SceneRendererData.TransformUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::TransformData), 1);

		s_SceneRendererData.DirLightUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::DirLightData), 2);
		//s_SceneRendererData.LightUniformBuffer     = UniformBuffer::Create(sizeof(SceneRendererData::LightData), 2);
		s_SceneRendererData.MaterialUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::MaterialData), 3);
	

		s_SceneRendererData.ViewportSize.x = width;
		s_SceneRendererData.ViewportSize.y = height;

		// Create white texture
		s_SceneRendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_SceneRendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// Arrange Framebuffers


		// Depth framebuffer for Spot and Directional Lights
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


		// -------------------- CALCULATE CAMERA DATA -------------------------------- //
			s_SceneRendererData.CameraBuffer.ViewProjection = camera.GetViewProjection();
			s_SceneRendererData.CameraBuffer.ViewPos = camera.GetPosition();
			s_SceneRendererData.CameraUniformBuffer->SetData(&s_SceneRendererData.CameraBuffer, sizeof(SceneRendererData::CameraData));
		// ---------------------- END CALCULATE CAMERA DATA -------------------------- //

		// ------------------ FILL IN DIR LIGHT UNIFORMS -------------------------- //
			glm::vec3 dirLightPos(s_SceneRendererData.Epsilon, s_SceneRendererData.Epsilon, s_SceneRendererData.Epsilon);
			// THIS WILL BE REPLACED WITH ENTITY COMPONENT SYSTEM !!!!!
			s_SceneRendererData.DirLightsBuffer.Size = 3;
			// light1 props
			float light1_xmin = -50.0f;
			float light1_xmax =  50.0f;
			float light1_ymin = -50.0f;
			float light1_ymax =  50.0f;
			float light1_zmin = -50.0f;
			float light1_zmax =  50.0f;
			glm::vec3 light1_color(0.8f, 0.86f, 0.3f); // yellow
			glm::vec3 light1_dir(0.0f, -1.0f, 0.0f);
			glm::mat4 light1_proj = glm::ortho(light1_xmin, light1_xmax, light1_ymin, light1_ymax, light1_zmin, light1_zmax);
			s_SceneRendererData.DirLightsBuffer.DirLights[0].Color = light1_color;
			s_SceneRendererData.DirLightsBuffer.DirLights[0].LightDir = glm::normalize(light1_dir);
			glm::mat4 light1_view = glm::lookAt(dirLightPos, light1_dir, glm::vec3(0.0f, 1.0f, 0.0f));
			s_SceneRendererData.DirLightsBuffer.DirLights[0].LightSpaceMatrix = light1_proj * light1_view;
			
			// light2 props
			float light2_xmin = -50.0f;
			float light2_xmax = 50.0f;
			float light2_ymin = -50.0f;
			float light2_ymax = 50.0f;
			float light2_zmin = -50.0f;
			float light2_zmax = 50.0f;
			glm::vec3 light2_color(0.5f, 0.5f, 0.5f); // dimmed white
			glm::vec3 light2_dir(-1.0f, -1.0f, -1.0f);
			glm::mat4 light2_proj = glm::ortho(light2_xmin, light2_xmax, light2_ymin, light2_ymax, light2_zmin, light2_zmax);
			s_SceneRendererData.DirLightsBuffer.DirLights[1].Color = light2_color;
			s_SceneRendererData.DirLightsBuffer.DirLights[1].LightDir = glm::normalize(light2_dir);
			glm::mat4 light2_view = glm::lookAt(dirLightPos, light2_dir, glm::vec3(0.0f, 1.0f, 0.0f));
			s_SceneRendererData.DirLightsBuffer.DirLights[1].LightSpaceMatrix = light2_proj * light2_view;


			// light3 props
			float light3_xmin = -50.0f;
			float light3_xmax = 50.0f;
			float light3_ymin = -50.0f;
			float light3_ymax = 50.0f;
			float light3_zmin = -50.0f;
			float light3_zmax = 50.0f;
			glm::vec3 light3_color(0.1f, 0.3f, 0.6f); // dimmed cyanish
			glm::vec3 light3_dir(1.0f, -1.0f, -1.0f);
			glm::mat4 light3_proj = glm::ortho(light3_xmin, light3_xmax, light3_ymin, light3_ymax, light3_zmin, light3_zmax);
			s_SceneRendererData.DirLightsBuffer.DirLights[2].Color = light3_color;
			s_SceneRendererData.DirLightsBuffer.DirLights[2].LightDir = glm::normalize(light3_dir);
			glm::mat4 light3_view = glm::lookAt(dirLightPos, light3_dir, glm::vec3(0.0f, 1.0f, 0.0f));
			s_SceneRendererData.DirLightsBuffer.DirLights[2].LightSpaceMatrix = light3_proj * light3_view;
		// ------------------- END FILL IN DIR LIGHT UNIFORMS --------------------- //



		// Render depth of scene to texture (from light's perspective)
		// -----------------------------------------------------------
		/*glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 0.2f, far_plane = 505.0f;
		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
		s_SceneRendererData.lightPos = glm::vec3(s_SceneRendererData.Epsilon, s_SceneRendererData.Epsilon, s_SceneRendererData.Epsilon);//glm::vec3(-2 + 9 * sin(glm::radians(time * 90)), 10.0f, -1 + 9 * cos(glm::radians(time * 90)));
		glm::mat3 rotationMatrix = glm::mat3(cos(0.001), sin(0.001), 0, -sin(0.001), cos(0.001f), 0, 0, 0, 1);
		s_SceneRendererData.LightBuffer.LightDir = rotationMatrix * s_SceneRendererData.LightBuffer.LightDir;
		lightView = glm::lookAt(s_SceneRendererData.lightPos, glm::normalize(s_SceneRendererData.LightBuffer.LightDir), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		
		

		s_SceneRendererData.LightBuffer.LightSpaceMatrix = lightSpaceMatrix;
		s_SceneRendererData.LightUniformBuffer->SetData(&s_SceneRendererData.LightBuffer, sizeof(SceneRendererData::LightData));
		*/

		glm::mat4 model = glm::mat4(1.0f);

		
		s_SceneRendererData.DirLightUniformBuffer->SetData(&s_SceneRendererData.DirLightsBuffer,
			sizeof(int) +
			sizeof(s_SceneRendererData.DirLightsBuffer.DirLights) * s_SceneRendererData.DirLightsBuffer.Size);

		s_SceneRendererData.depthShaderPass->InvokeCommands(
			[&] () -> void {

				s_SceneRendererData.depthShader->Bind();
				glViewport(0, 0, s_SceneRendererData.ViewportSize.x, s_SceneRendererData.ViewportSize.y);

				glClear(GL_DEPTH_BUFFER_BIT);
				glCullFace(GL_FRONT);
				// ------------ DRAW SCENE ------------


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.icosphere->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
				model = glm::scale(model, glm::vec3(60.0f, 0.5f, 60.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 2.0f));
				model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.cube->Draw();


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(5.0f, 3.0f, -2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.icosphere2->Draw();



				s_SceneRendererData.spinningModel = glm::rotate(s_SceneRendererData.spinningModel, glm::radians((float)ts * time * 15.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
				s_SceneRendererData.TransformBuffer.Model = s_SceneRendererData.spinningModel;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
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
				s_SceneRendererData.TransformBuffer.Model = model;
				// yellowish
				s_SceneRendererData.MaterialBuffer.Color = glm::vec3(0.7f, 0.6f, 0.2f);
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.icosphere->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
				model = glm::scale(model, glm::vec3(60.0f, 0.5f, 60.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));;
				s_SceneRendererData.TransformBuffer.Model = model;
				// bright grey
				s_SceneRendererData.MaterialBuffer.Color = glm::vec3(0.9f, 0.9f, 0.9f);
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 2.0f));
				model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				// dark orange
				s_SceneRendererData.MaterialBuffer.Color = glm::vec3(0.8f, 0.2f, 0.4f);
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(5.0f, 3.0f, -2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				// dark orange
				s_SceneRendererData.MaterialBuffer.Color = glm::vec3(0.1f, 0.8f, 0.4f);
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.icosphere2->Draw();


				s_SceneRendererData.spinningModel = glm::rotate(s_SceneRendererData.spinningModel, glm::radians((float)ts * time * 15.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
				s_SceneRendererData.TransformBuffer.Model = s_SceneRendererData.spinningModel;
				// dark orange
				s_SceneRendererData.MaterialBuffer.Color = glm::vec3(0.1f, 0.5f, 0.9f);
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.icosphere3->Draw();
				// ---------- DRAW SCENE END ----------
			}
		);

	}

	SceneRendererStats SceneRenderer::GetStats()
	{
		return SceneRendererStats();
	}

}