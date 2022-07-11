#include <Precomp.h>
#include "SceneRenderer.h"

#include <Math/Math.h>

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
#define MAX_CASCADE_SIZE 5

namespace OP
{


	// ---------------------------- UTILITY FUNCTIONS ------------------------------------ //
		
		// ----------- CASCADED SHADOW MAPPING ------------ //

			// Finds world coordinates of the frustum represented by
			// the projection matrix
			// We will use camera's projection matrix in this function
			static std::vector<glm::vec4> GetFrustumCornerCoordinatesWorldSpace(const glm::mat4& projViewMatrix)
			{
				const glm::mat4 inv = glm::inverse(projViewMatrix);

				std::vector<glm::vec4> frustumCoordinates;
				for (uint32_t x = 0; x < 2; x++)
				{
					for (uint32_t y = 0; y < 2; y++)
					{
						for (uint32_t z = 0; z < 2; z++)
						{
							const glm::vec4 ndcPoint = inv * glm::vec4(2.0f * x - 1.0f,
																	   2.0f * y - 1.0f,
																	   2.0f * z - 1.0f,
																	   1.0f);

							// Perspective division
							frustumCoordinates.push_back(ndcPoint / ndcPoint.w);
						}
					}
				}

				return frustumCoordinates;
			}

			// static std::vector<glm::vec4> GetFrustumCornerCoordinatesWorldSpace(const glm::mat4& projectionMatrix, const glm::mat4& view)
			// {
			//	return GetFrustomCornerCoordinatesWorldSpace(projectionMatrix * view);
			// }

			// We will get orthographic projection matrix which tightly overlaps the frustum part and
			// view matrix of the light (light will be on the center of frustum)
			static glm::mat4 GetLightLightSpaceMatrix(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const float nearPlane, const float farPlane, const glm::vec3& lightDir, const float zMult)
			{

				float left_, right_, bottom_, top_, near_, far_;

				
				Math::DecomposePerspectiveProj(projectionMatrix, left_, right_, bottom_, top_, near_, far_);

				float fovy = 2 * std::atan2(top_, near_);
				float aspectRatio = right_ / top_;

				glm::mat4 newProjectionMatrix = glm::perspective(fovy, aspectRatio, nearPlane, farPlane);

				std::vector<glm::vec4> frustumCornerCoordinates = GetFrustumCornerCoordinatesWorldSpace(newProjectionMatrix * viewMatrix);

				// Calculate view matrix
				glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
				for (const glm::vec4& coord : frustumCornerCoordinates)
				{
					center += glm::vec3(coord);
				}
				center /= frustumCornerCoordinates.size();

				glm::vec3 controlledLightDir = lightDir;
				if (controlledLightDir.x == 0.0 && controlledLightDir.z == 0.0)
				{
					controlledLightDir += glm::vec3(0.001f, 0.0f, -0.001f);
				}
				glm::mat4 lightView = glm::lookAt(center, center + controlledLightDir, glm::vec3(0.0f, 1.0f, 0.0f));

				float minX = std::numeric_limits<float>::max();
				float maxX = std::numeric_limits<float>::min();
				float minY = std::numeric_limits<float>::max();
				float maxY = std::numeric_limits<float>::min();
				float minZ = std::numeric_limits<float>::max();
				float maxZ = std::numeric_limits<float>::min();

				// Determine box bounds by traversing frustum coordinates
				for (const glm::vec4& coord : frustumCornerCoordinates)
				{
					const glm::vec4 coordAccordingToLight = lightView * coord;
					minX = std::min(minX, coordAccordingToLight.x);
					maxX = std::max(maxX, coordAccordingToLight.x);
					minY = std::min(minY, coordAccordingToLight.y);
					maxY = std::max(maxY, coordAccordingToLight.y);
					minZ = std::min(minZ, coordAccordingToLight.z);
					maxZ = std::max(maxZ, coordAccordingToLight.z);
					
					if (isnan(coordAccordingToLight.x) || isnan(coordAccordingToLight.y) || isnan(coordAccordingToLight.z))
						OP_ENGINE_INFO("isnan!");
				}

				// Tune parameter
				if (minZ < 0)
					minZ *= zMult;
				else
					minZ /= zMult;
				if (maxZ < 0)
					maxZ /= zMult;
				else
					maxZ *= zMult;

				const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

				return lightProjection * lightView;
			}

			// This function gives cascaded levels according to a distFactor.
			// The larger this factor is, further levels have much more far - near differences
			// If distributionFactor is 1, then far plane is divided equally.
			static std::vector<float> DistributeShadowCascadeLevels(uint32_t cascadeSize, float distFactor, float farPlane)
			{
				const uint32_t newPlaneSize = cascadeSize - 1;

				std::vector<float> levels;

				if (newPlaneSize == 0)
				{
					levels.push_back(farPlane);
					return levels;
				}

				float unitSize = farPlane / std::pow(cascadeSize, distFactor);
				levels.push_back(unitSize);

				float prevSum = 1.0f;
				for (uint32_t i = 2; i < newPlaneSize + 1; i++)
				{
					float newSum = std::pow(i, distFactor);
					levels.push_back(unitSize * (newSum));
					prevSum = newSum;
				}
				
				return levels;
			}


			// It gives light space matrices for each cascade
			std::vector<glm::mat4> getLightSpaceMatrices(const glm::mat4& projectionMatrix,
				                                         const glm::mat4& viewMatrix,
				                                         const std::vector<float>& cascadeLevels,
				                                         const float nearPlane, const float farPlane,
				                                         const glm::vec3& lightDir, const float zMult)
			{
				std::vector<glm::mat4> matrices;

				for (uint32_t i = 0; i < cascadeLevels.size(); i++)
				{
					if (i == 0)
					{
						matrices.push_back(GetLightLightSpaceMatrix(projectionMatrix, viewMatrix, nearPlane, cascadeLevels[i], lightDir, zMult));
					}
					else
					{
						matrices.push_back(GetLightLightSpaceMatrix(projectionMatrix, viewMatrix, cascadeLevels[i - 1], cascadeLevels[i], lightDir, zMult));
					}
				}

				if(cascadeLevels.size() > 1)
					matrices.push_back(GetLightLightSpaceMatrix(projectionMatrix, viewMatrix, cascadeLevels[cascadeLevels.size() - 1], farPlane, lightDir, zMult));

				return matrices;
			}



		// -------- END CASCADED SHADOW MAPPING ----------- //




	struct SceneRendererData
	{
		// --------- RENDERER CONSTANTS -------- //
			float Epsilon = 0.00000001;
		// ------ END RENDER CONSTANTS --------- //

		// ----------- UNIFORM BUFFERS --------- //

			struct CameraData
			{
				glm::mat4 ViewProjection;
				glm::mat4 View;
				alignas(16) glm::vec3 ViewPos;
			} CameraBuffer;

			Ref<UniformBuffer> CameraUniformBuffer;

			struct TransformData
			{
				glm::mat4 Model;
			} TransformBuffer;

			Ref<UniformBuffer> TransformUniformBuffer;

		// ------ END UNIFORM BUFFERS ---------- //



		// -------- CASCADED SHADOW MAPPING SETTINGS ------- //
			float zMult = 10;
			float shadowMapResX = 4096;
			float shadowMapResY = 4096;

		// ----- END CASCADED SHADOW MAPPING SETTINGS ------ //

			// LightSpace Matrices obtained from directional and spot lights
			struct LightSpaceMatricesDSData
			{
				int dummy;
				glm::mat4 LightSpaceMatricesDirSpot[MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS];

			} LightSpaceMatricesDSBuffer;


			struct CascadePlaneDistancesData
			{
				alignas(4) int dummy;
				float cascadePlaneDistances[(MAX_CASCADE_SIZE - 1) * MAX_DIR_LIGHTS];
			} CascadePlaneDistancesBuffer;
			
			Ref<UniformBuffer> LightSpaceMatricesDSUniformBuffer;
			Ref<UniformBuffer> CascadePlaneDistancesUniformBuffer;

		// --------


		// --------- DIRECTIONAL LIGHT ------------- //
		struct DirLight
		{
			int CascadeSize;
			int FrustaDistFactor;
			alignas(16) glm::vec3 LightDir;
			alignas(16) glm::vec3 Color;
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

		s_SceneRendererData.icosphere = Icosphere::Create(1.0f, 4, true);
		s_SceneRendererData.icosphere2 = Icosphere::Create(3.0f, 0, false);
		s_SceneRendererData.icosphere3 = Icosphere::Create(1.5f, 1, false);
		s_SceneRendererData.cube = Cube::Create();
		s_SceneRendererData.plane = Plane::Create();

		s_SceneRendererData.mainShader = Shader::Create("assets/shaders/Pbr/Main.glsl");
		s_SceneRendererData.depthShader = Shader::Create("assets/shaders/Pbr/DirSpotShadowMapping.glsl");
		s_SceneRendererData.depthDebugShader = Shader::Create("assets/shaders/Pbr/DepthDebug.glsl");


		// Deal with uniform buffers
		s_SceneRendererData.CameraUniformBuffer    = UniformBuffer::Create(sizeof(SceneRendererData::CameraData), 0);
		s_SceneRendererData.TransformUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::TransformData), 1);

		s_SceneRendererData.DirLightUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::DirLightData), 2);
		s_SceneRendererData.LightSpaceMatricesDSUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::LightSpaceMatricesDSData), 3);
		s_SceneRendererData.CascadePlaneDistancesUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::CascadePlaneDistancesData), 4);

		s_SceneRendererData.MaterialUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::MaterialData), 5);
	

		s_SceneRendererData.ViewportSize.x = width;
		s_SceneRendererData.ViewportSize.y = height;

		// Create white texture
		s_SceneRendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_SceneRendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// Arrange Framebuffers


		// Depth framebuffer for Spot and Directional Lights
		FramebufferSpecification depthFBSpec;
		depthFBSpec.Attachments = { FramebufferTextureFormat::SHADOWMAP_ARRAY_DEPTH };
		depthFBSpec.Width = s_SceneRendererData.shadowMapResX;
		depthFBSpec.Height = s_SceneRendererData.shadowMapResY;
		// s_SceneRendererData.depthFramebuffer = Framebuffer::Create(depthFBSpec);
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
			// s_SceneRendererData.depthFramebuffer->Resize(width, height);
			s_SceneRendererData.finalFramebuffer->Resize(width, height);

			// s_SceneRendererData.depthShaderPass->ResizeFramebuffer(width, height);
		}
	}

	void SceneRenderer::Render(EditorCamera& camera, Timestep ts)
	{

		float time = (float)Application::Get().GetWindow().GetTime();
		glm::mat3 rotationMatrix = glm::mat3(cos(0.001), sin(0.001), 0, -sin(0.001), cos(0.001f), 0, 0, 0, 1);

		// -------------------- CALCULATE CAMERA DATA -------------------------------- //
			s_SceneRendererData.CameraBuffer.ViewProjection = camera.GetViewProjection();
			s_SceneRendererData.CameraBuffer.ViewPos = camera.GetPosition();
			s_SceneRendererData.CameraBuffer.View = camera.GetViewMatrix();
			s_SceneRendererData.CameraUniformBuffer->SetData(&s_SceneRendererData.CameraBuffer, sizeof(SceneRendererData::CameraData));
		// ---------------------- END CALCULATE CAMERA DATA -------------------------- //

		// ------------------ FILL IN DIR LIGHT UNIFORMS -------------------------- //
			glm::mat4 cameraProjection = camera.GetProjection();
			glm::vec3 dirLightPos(s_SceneRendererData.Epsilon, s_SceneRendererData.Epsilon, s_SceneRendererData.Epsilon);
			// THIS WILL BE REPLACED WITH ENTITY COMPONENT SYSTEM !!!!!
			s_SceneRendererData.DirLightsBuffer.Size = 1;
			// light1 props
			glm::vec3 light1_color(0.2f, 0.26f, 0.2f);
			glm::vec3 light1_dir(0.0f, -1.1f, 0.0f);
			int light1CascadeSize = 3;
			float light1FrustaDistFactor = 1;
			s_SceneRendererData.DirLightsBuffer.DirLights[0].Color = light1_color;
			s_SceneRendererData.DirLightsBuffer.DirLights[0].LightDir = glm::normalize(light1_dir);
			s_SceneRendererData.DirLightsBuffer.DirLights[0].CascadeSize = light1CascadeSize;
			s_SceneRendererData.DirLightsBuffer.DirLights[0].FrustaDistFactor = light1FrustaDistFactor;
			std::vector<float> cascadeLevels = DistributeShadowCascadeLevels(light1CascadeSize, light1FrustaDistFactor, camera.GetFarClip());
			OP_ENGINE_ASSERT(cascadeLevels <= MAX_CASCADE_SIZE);
			std::vector<glm::mat4> matrices = getLightSpaceMatrices(cameraProjection, camera.GetViewMatrix(), cascadeLevels, camera.GetNearClip(), camera.GetFarClip(), light1_dir, s_SceneRendererData.zMult);
			for (uint32_t i = 0; i < matrices.size(); i++)
			{
				s_SceneRendererData.LightSpaceMatricesDSBuffer.LightSpaceMatricesDirSpot[MAX_CASCADE_SIZE * 0 + i] = matrices[i];
			}
			for (uint32_t i = 0; i < cascadeLevels.size(); i++)
			{
				s_SceneRendererData.CascadePlaneDistancesBuffer.cascadePlaneDistances[(MAX_CASCADE_SIZE - 1) * 0 + i] = cascadeLevels[i];
			}
			s_SceneRendererData.CascadePlaneDistancesBuffer.dummy = 2;
			s_SceneRendererData.LightSpaceMatricesDSBuffer.dummy = 2;
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!
			// Sonraki isiklardan devam et
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!

			// light2 props
			/*glm::vec3 light2_color(0.2f, 0.2f, 0.6f);
			glm::vec3 light2_dir(-1.0f, -1.0f, -0.0f);
			int light2CascadeSize = 4;
			float light2FrustaDistFactor = 1;
			s_SceneRendererData.DirLightsBuffer.DirLights[1].Color = light2_color;
			s_SceneRendererData.DirLightsBuffer.DirLights[1].LightDir = glm::normalize(light2_dir);
			s_SceneRendererData.DirLightsBuffer.DirLights[1].CascadeSize = light2CascadeSize;
			s_SceneRendererData.DirLightsBuffer.DirLights[1].FrustaDistFactor = light2FrustaDistFactor;
			std::vector<float> cascadeLevels2 = DistributeShadowCascadeLevels(light2CascadeSize, light2FrustaDistFactor, camera.GetFarClip());
			OP_ENGINE_ASSERT(cascadeLevels2 <= MAX_CASCADE_SIZE);
			std::vector<glm::mat4> matrices2 = getLightSpaceMatrices(cameraProjection, camera.GetViewMatrix(), cascadeLevels2, camera.GetNearClip(), camera.GetFarClip(), light2_dir, s_SceneRendererData.zMult);
			for (uint32_t i = 0; i < matrices2.size(); i++)
			{
				s_SceneRendererData.LightSpaceMatricesDSBuffer.LightSpaceMatricesDirSpot[MAX_CASCADE_SIZE * 1 + i] = matrices[i];
			}
			for (uint32_t i = 0; i < cascadeLevels2.size(); i++)
			{
				s_SceneRendererData.CascadePlaneDistancesBuffer.cascadePlaneDistances[(MAX_CASCADE_SIZE - 1) * 1 + i] = cascadeLevels2[i];
			}*/

			// light3 props
			/*
			glm::vec3 light3_color(0.5f, 0.1f, 0.6f);
			glm::vec3 light3_dir(1.0f, -1.0f, -1.0f);
			int light3CascadeSize = 4;
			float light3FrustaDistFactor = 2;
			s_SceneRendererData.DirLightsBuffer.DirLights[2].Color = light3_color;
			s_SceneRendererData.DirLightsBuffer.DirLights[2].LightDir = glm::normalize(light3_dir);
			s_SceneRendererData.DirLightsBuffer.DirLights[2].CascadeSize = light3CascadeSize;
			s_SceneRendererData.DirLightsBuffer.DirLights[2].FrustaDistFactor = light3FrustaDistFactor;
			std::vector<float> cascadeLevels3 = DistributeShadowCascadeLevels(light3CascadeSize, light3FrustaDistFactor, camera.GetFarClip());
			OP_ENGINE_ASSERT(cascadeLevels3 <= MAX_CASCADE_SIZE);
			std::vector<glm::mat4> matrices3 = getLightSpaceMatrices(cameraProjection, camera.GetViewMatrix(), cascadeLevels3, camera.GetNearClip(), camera.GetFarClip(), light3_dir, s_SceneRendererData.zMult);
			for (uint32_t i = 0; i < matrices3.size(); i++)
			{
				s_SceneRendererData.LightSpaceMatricesDSBuffer.LightSpaceMatricesDirSpot[MAX_CASCADE_SIZE * 2 + i] = matrices[i];
			}

			for (uint32_t i = 0; i < cascadeLevels3.size(); i++)
			{
				s_SceneRendererData.CascadePlaneDistancesBuffer.cascadePlaneDistances[(MAX_CASCADE_SIZE - 1) * 2 + i] = cascadeLevels3[i];
			}*/
		// ------------------- END FILL IN DIR LIGHT UNIFORMS --------------------- //



		glm::mat4 model = glm::mat4(1.0f);

		
		s_SceneRendererData.DirLightUniformBuffer->SetData(&s_SceneRendererData.DirLightsBuffer,
			sizeof(s_SceneRendererData.DirLightsBuffer));

		s_SceneRendererData.LightSpaceMatricesDSUniformBuffer->SetData(&s_SceneRendererData.LightSpaceMatricesDSBuffer,
			sizeof(s_SceneRendererData.LightSpaceMatricesDSBuffer));

		s_SceneRendererData.CascadePlaneDistancesUniformBuffer->SetData(&s_SceneRendererData.CascadePlaneDistancesBuffer,
			sizeof(s_SceneRendererData.CascadePlaneDistancesBuffer));

		s_SceneRendererData.depthShaderPass->InvokeCommands(
			[&] () -> void {

				s_SceneRendererData.depthShader->Bind();
				glViewport(0, 0, s_SceneRendererData.shadowMapResX, s_SceneRendererData.shadowMapResY);

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
				// reset viewport
				glViewport(0, 0, s_SceneRendererData.ViewportSize.x, s_SceneRendererData.ViewportSize.y);

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