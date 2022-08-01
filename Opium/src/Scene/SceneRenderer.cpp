#include <Precomp.h>
#include "SceneRenderer.h"

#include <Math/Math.h>

#include <Opium/Application.h>
#include <Opium/ResourceManager.h>

#include <Renderer/VertexArray.h>
#include <Renderer/Shader.h>
#include <Renderer/UniformBuffer.h>
#include <Renderer/RenderCommand.h>
#include <Renderer/PingPongRenderPass.h>

#include <Renderer/Texture.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Temp

#include <glad/glad.h>

#include <Geometry/Icosphere.h>
#include <Geometry/Cube.h>
#include <Geometry/Plane.h>
#include <Geometry/Quad.h>
#include <Geometry/Model.h>

#define MAX_DIR_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADE_SIZE 10
#define MAX_POINT_LIGHTS 10
#define MAX_BONES 100

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
					controlledLightDir += glm::vec3(0.00001f, 0.0f, -0.00001f);
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


		// -------- LIGHT ATTENUATION COEFF CALCULATION --------- //

			// Gives attenuation coefficients for point and spot lights
			static void GiveCoeff(float& kq, float& kl, float mult, float range)
			{
				kq = 0.001f / (range * range + mult * range);
				kl = mult * kq;
			}
				


		// ---- END LIGHT ATTENUATION COEFF CALCULATION --------- //


	struct SceneRendererData
	{

		Ref<Scene> m_Context;

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

		// ------ Animation Uniform Buffer ----- //

			struct BoneMatrix
			{
				glm::mat4 mat;
			};

			// Bone matrices data
			struct BoneMatricesData
			{
				BoneMatrix BoneMatrices[MAX_BONES];

			};

			Ref<UniformBuffer> BoneMatricesUniformBuffer;
		// ------------------------------------- //


		// -------- CASCADED SHADOW MAPPING SETTINGS ------- //
			float zMult = 10;
			float blurAmount = 1;
			struct ShadowMapSettings
			{
				float shadowMapResX = 512.0f;
				float shadowMapResY = 512.0f;
				float pointLightSMResX = 1024.0f;
				float pointLightSMResY = 1024.0f;
				glm::vec2 blurScale = glm::vec2(0.0f);
			} ShadowMapSettingsBuffer;

			Ref<UniformBuffer> ShadowMapSettingsUniformBuffer;

			struct ToneMappingSettings
			{
				float exposure = 1.0;
				bool hdr = true;
			} ToneMappingSettingsBuffer;

			Ref<UniformBuffer> ToneMappingSettingsUniformBuffer;

		// ----- END CASCADED SHADOW MAPPING SETTINGS ------ //

			struct LightSpaceMatrix
			{
				glm::mat4 mat;
			};

			// LightSpace Matrices obtained from directional and spot lights
			struct LightSpaceMatricesDSData
			{
				LightSpaceMatrix LightSpaceMatricesDirSpot[MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS];

			} LightSpaceMatricesDSBuffer;

			// LightSpace Matrices obtained from point lights
			struct LightSpaceMatricesPointData
			{
				LightSpaceMatrix LightSpaceMatricesPoint[MAX_POINT_LIGHTS * 6];
			} LightSpaceMatricesPointBuffer;

			struct CascadePlane
			{
				float dist;
				float _align1;
				float _align2;
				float _align3;
			};

			struct CascadePlaneDistancesData
			{
				CascadePlane cascadePlanes[(MAX_CASCADE_SIZE - 1) * MAX_DIR_LIGHTS];
			} CascadePlaneDistancesBuffer;
			

			Ref<UniformBuffer> LightSpaceMatricesDSUniformBuffer;
			Ref<UniformBuffer> LightSpaceMatricesPointUniformBuffer;
			Ref<UniformBuffer> CascadePlaneDistancesUniformBuffer;

		// --------


		// --------- DIRECTIONAL LIGHT ------------- //
		struct DirLight
		{
			int CascadeSize;
			float FrustaDistFactor;
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
			float Cutoff;
			float OuterCutoff;
			float NearDist;
			float FarDist;
			float Bias;
			float Kq;
			float Kl;
			alignas(16) glm::vec3 LightDir;
			alignas(16) glm::vec3 Color;
			alignas(16) glm::vec3 Position;
		};

		struct SpotLightData
		{
			int Size;
			SpotLight SpotLights[MAX_SPOT_LIGHTS];
		} SpotLightsBuffer;

		Ref<UniformBuffer> SpotLightUniformBuffer;
		// --------- END SPOT LIGHT ------------ //

		// ----------- POINT LIGHT ------------- //
		struct PointLight
		{
			float NearDist;
			float FarDist;
			float Kq;
			float Kl;
			alignas(16) glm::vec3 Color;
			alignas(16) glm::vec3 Position;
		};

		struct PointLightData
		{
			int Size;
			PointLight PointLights[MAX_POINT_LIGHTS];
		} PointLightsBuffer;

		Ref<UniformBuffer> PointLightUniformBuffer;
		// ------------------------------------- //




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
		Ref<Shader> pointLightDepthShader;
		Ref<Shader> depthDebugShader;
		Ref<Shader> shadowMapDirSpotBlur;
		Ref<Shader> pointLightSMBlurShader;
		Ref<Shader> postProcessingShader;

		// Ref<Texture2D> woodTexture;
		Ref<Texture2D> WhiteTexture;

		Ref<Cube> cube;
		Ref<Plane> plane;
		Ref<Quad> quad;

		Ref<Model> animatedModel;

		// Ref<Icosphere> icosphere2;


		// Directional light
		glm::vec3 lightPos{-2.0f, 10.0f, -1.0f};

		glm::vec2 ViewportSize{0.0f, 0.0f};


		// Framebuffer
		Ref<Framebuffer> depthFramebuffer;
		Ref<Framebuffer> finalFramebuffer;
		Ref<Framebuffer> sampleResolveFramebuffer;


		Ref<RenderPass> depthRenderPass;
		Ref<RenderPass> pointLightDepthRenderPass;
		Ref<PingPongRenderPass> depthBlurDSLRenderPass;
		Ref<RenderPass> finalRenderPass;
		Ref<RenderPass> postProcessingPass;

		// Temp
		glm::mat4 spinningModel;
		glm::vec3 spinningDir;

	};


	static SceneRendererData s_SceneRendererData;

	void SceneRenderer::Init(float width, float height, Ref<Framebuffer> fB)
	{

		// ---------- TEMP ----------

		s_SceneRendererData.spinningModel = glm::mat4(1.0f);
		s_SceneRendererData.spinningModel = glm::translate(s_SceneRendererData.spinningModel, glm::vec3(-4.0f, 6.0f, 5.0f));
		s_SceneRendererData.spinningModel = glm::scale(s_SceneRendererData.spinningModel, glm::vec3(1.0f, 1.0f, 1.0f));
		s_SceneRendererData.LightBuffer.LightDir = glm::vec3(0.0f, -1.0f, 0.0f);

		s_SceneRendererData.spinningDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.0f));
		// ---------------

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		s_SceneRendererData.cube = Cube::Create();
		s_SceneRendererData.plane = Plane::Create();
		s_SceneRendererData.quad = Quad::Create();


		s_SceneRendererData.shadowMapDirSpotBlur = ResourceManager::GetShader("DirSpotShadowMappingBlur.glsl");
		s_SceneRendererData.mainShader = ResourceManager::GetShader("Main.glsl");
		s_SceneRendererData.depthShader = ResourceManager::GetShader("DirSpotShadowMapping.glsl");
		s_SceneRendererData.depthDebugShader = ResourceManager::GetShader("DepthDebug.glsl");
		s_SceneRendererData.pointLightDepthShader = ResourceManager::GetShader("PointShadowMapping.glsl");
		s_SceneRendererData.postProcessingShader = ResourceManager::GetShader("PostProcessing.glsl");


		s_SceneRendererData.animatedModel = ResourceManager::GetModel("Thriller Part 1");

		// Deal with uniform buffers
		s_SceneRendererData.CameraUniformBuffer            = UniformBuffer::Create(sizeof(SceneRendererData::CameraData), 0);
		s_SceneRendererData.TransformUniformBuffer         = UniformBuffer::Create(sizeof(SceneRendererData::TransformData), 1);
		s_SceneRendererData.ShadowMapSettingsUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::ShadowMapSettings), 2);

		s_SceneRendererData.DirLightUniformBuffer   = UniformBuffer::Create(sizeof(SceneRendererData::DirLightData), 3);
		s_SceneRendererData.SpotLightUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::SpotLightData), 4);
		s_SceneRendererData.PointLightUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::PointLightData), 5);

		s_SceneRendererData.LightSpaceMatricesDSUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::LightSpaceMatricesDSData), 6);
		s_SceneRendererData.LightSpaceMatricesPointUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::LightSpaceMatricesPointData), 7);
		s_SceneRendererData.CascadePlaneDistancesUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::CascadePlaneDistancesData), 8);

		s_SceneRendererData.MaterialUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::MaterialData), 9);

		s_SceneRendererData.ToneMappingSettingsUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::ToneMappingSettings), 10);


		s_SceneRendererData.BoneMatricesUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::BoneMatricesData), 11);

		s_SceneRendererData.ViewportSize.x = width;
		s_SceneRendererData.ViewportSize.y = height;

		// Create white texture
		s_SceneRendererData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_SceneRendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// Arrange Framebuffers


		// Depth framebuffer for Spot and Directional Lights
		FramebufferSpecification depthFBSpec;
		depthFBSpec.Attachments = { FramebufferTextureFormat::SM_VARIANCE32F, FramebufferTextureFormat::SHADOWMAP_ARRAY_DEPTH };
		depthFBSpec.Width = s_SceneRendererData.ShadowMapSettingsBuffer.shadowMapResX;
		depthFBSpec.Height = s_SceneRendererData.ShadowMapSettingsBuffer.shadowMapResY;
		// s_SceneRendererData.depthFramebuffer = Framebuffer::Create(depthFBSpec);
		s_SceneRendererData.depthRenderPass = RenderPass::Create(std::string("Depth Pass"), depthFBSpec, s_SceneRendererData.depthShader);

		// Also set shadow map size in shadow map settings uniform buffer
		s_SceneRendererData.ShadowMapSettingsUniformBuffer->SetData(&s_SceneRendererData.ShadowMapSettingsBuffer, sizeof(SceneRendererData::ShadowMapSettings));


		s_SceneRendererData.depthBlurDSLRenderPass = PingPongRenderPass::Create(std::string("DSL Blur Pass"), depthFBSpec, s_SceneRendererData.shadowMapDirSpotBlur);

		// Depth framebuffer for Point Lights
		FramebufferSpecification depthFBPointLight;
		depthFBPointLight.Attachments = { FramebufferTextureFormat::CUBEMAP_ARRAY_DEPTH };
		depthFBPointLight.Width = s_SceneRendererData.ShadowMapSettingsBuffer.pointLightSMResX;
		depthFBPointLight.Height = s_SceneRendererData.ShadowMapSettingsBuffer.pointLightSMResY;

		s_SceneRendererData.pointLightDepthRenderPass = RenderPass::Create(std::string("Point Light Depth Pass"), depthFBPointLight, s_SceneRendererData.depthShader);



		// Final Framebuffer
		FramebufferSpecification finalFBSpec;
		finalFBSpec.Attachments = { FramebufferTextureFormat::RGBA32F, FramebufferTextureFormat::Depth };
		finalFBSpec.Width = s_SceneRendererData.ViewportSize.x;
		finalFBSpec.Height = s_SceneRendererData.ViewportSize.y;
		finalFBSpec.Samples = 4;
		s_SceneRendererData.finalFramebuffer = Framebuffer::Create(finalFBSpec);

		s_SceneRendererData.finalRenderPass = RenderPass::Create(std::string("Final Pass"), finalFBSpec);


		// Sample Resolve Framebuffer - Intermediate
		FramebufferSpecification sampleResolveFB;
		sampleResolveFB.Attachments = { FramebufferTextureFormat::RGBA32F, FramebufferTextureFormat::Depth };
		sampleResolveFB.Width = s_SceneRendererData.ViewportSize.x;
		sampleResolveFB.Height = s_SceneRendererData.ViewportSize.y;
		s_SceneRendererData.sampleResolveFramebuffer = Framebuffer::Create(sampleResolveFB);


		// Post Processing Final Framebuffer
		FramebufferSpecification postProcessingFBSpec;
		postProcessingFBSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		postProcessingFBSpec.Width = s_SceneRendererData.ViewportSize.x;
		postProcessingFBSpec.Height = s_SceneRendererData.ViewportSize.y;

		s_SceneRendererData.postProcessingPass = RenderPass::Create(std::string("Post Processing Pass"), postProcessingFBSpec);

		s_SceneRendererData.finalFramebuffer = fB;



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
			s_SceneRendererData.postProcessingPass->GetFramebuffer()->Resize(width, height);
			s_SceneRendererData.sampleResolveFramebuffer->Resize(width, height);
			s_SceneRendererData.finalRenderPass->GetFramebuffer()->Resize(width, height);

			// s_SceneRendererData.finalFramebuffer->Resize(width, height);

			// s_SceneRendererData.depthRenderPass->ResizeFramebuffer(width, height);
		}
	}

	Ref<Framebuffer> SceneRenderer::GetFinalFramebuffer()
	{
		return s_SceneRendererData.postProcessingPass->GetFramebuffer();
	}

	void SceneRenderer::SetScene(Ref<Scene> scene)
	{
		s_SceneRendererData.m_Context = scene;
	}

	float* SceneRenderer::GetExposure()
	{
		return &s_SceneRendererData.ToneMappingSettingsBuffer.exposure;
	}

	bool* SceneRenderer::GetHdr()
	{
		return &s_SceneRendererData.ToneMappingSettingsBuffer.hdr;
	}

	void SceneRenderer::Render(EditorCamera& camera, Ref<Scene> scene, Timestep ts)
	{

		float time = (float)Application::Get().GetWindow().GetTime();
		glm::mat3 rotationMatrix = glm::mat3(cos(0.001), sin(0.001), 0, -sin(0.001), cos(0.001f), 0, 0, 0, 1);
		s_SceneRendererData.spinningDir = rotationMatrix * s_SceneRendererData.spinningDir;
		s_SceneRendererData.spinningDir = glm::normalize(s_SceneRendererData.spinningDir);
		// -------------------- CALCULATE CAMERA DATA -------------------------------- //
			s_SceneRendererData.CameraBuffer.ViewProjection = camera.GetViewProjection();
			s_SceneRendererData.CameraBuffer.ViewPos = camera.GetPosition();
			s_SceneRendererData.CameraBuffer.View = camera.GetViewMatrix();
			s_SceneRendererData.CameraUniformBuffer->SetData(&s_SceneRendererData.CameraBuffer, sizeof(SceneRendererData::CameraData));
		// ---------------------- END CALCULATE CAMERA DATA -------------------------- //


		// ---------------------- TONE MAPPING SETTINGS ------------------------------ //
			s_SceneRendererData.ToneMappingSettingsUniformBuffer->SetData(&s_SceneRendererData.ToneMappingSettingsBuffer, sizeof(SceneRendererData::ToneMappingSettings));


		// --------------------- UPDATE ANIMATIONS -------------------------------- //

			s_SceneRendererData.animatedModel->UpdateAnimation(ts);
			s_SceneRendererData.BoneMatricesUniformBuffer->SetData(&s_SceneRendererData.animatedModel->GetFinalBoneMatrices(), sizeof(SceneRendererData::BoneMatricesData));

		// ------------------ FILL IN DIR LIGHT UNIFORMS -------------------------- //
			glm::mat4 cameraProjection = camera.GetProjection();

			{
				auto group = scene->m_Registry.group<TransformComponent>(entt::get<DirLightComponent>);
				s_SceneRendererData.DirLightsBuffer.Size = group.size() < MAX_DIR_LIGHTS ? group.size() : MAX_DIR_LIGHTS;
				int dirLightCounter = 0;
				for (auto entity : group)
				{
					auto [transform, dirLight] = group.get<TransformComponent, DirLightComponent>(entity);

					glm::vec3 lightDirection = transform.GetDirection();
					s_SceneRendererData.DirLightsBuffer.DirLights[dirLightCounter].LightDir = lightDirection;
					s_SceneRendererData.DirLightsBuffer.DirLights[dirLightCounter].Color = dirLight.Color;
					s_SceneRendererData.DirLightsBuffer.DirLights[dirLightCounter].CascadeSize = dirLight.CascadeSize;
					s_SceneRendererData.DirLightsBuffer.DirLights[dirLightCounter].FrustaDistFactor = dirLight.FrustaDistFactor;

					std::vector<float> cascadeLevels = DistributeShadowCascadeLevels(dirLight.CascadeSize, dirLight.FrustaDistFactor, camera.GetFarClip());
					OP_ENGINE_ASSERT(cascadeLevels <= MAX_CASCADE_SIZE);
					std::vector<glm::mat4> matrices = getLightSpaceMatrices(cameraProjection, camera.GetViewMatrix(),
						cascadeLevels, camera.GetNearClip(), camera.GetFarClip(),
						lightDirection, s_SceneRendererData.zMult);

					for (uint32_t i = 0; i < matrices.size(); i++)
					{
						s_SceneRendererData.LightSpaceMatricesDSBuffer.LightSpaceMatricesDirSpot[MAX_CASCADE_SIZE * dirLightCounter + i].mat = matrices[i];
					}

					for (uint32_t i = 0; i < cascadeLevels.size(); i++)
					{
						s_SceneRendererData.CascadePlaneDistancesBuffer.cascadePlanes[(MAX_CASCADE_SIZE - 1) * dirLightCounter + i].dist = cascadeLevels[i];
					}

					dirLightCounter++;
				}
			}

			auto group2 = scene->m_Registry.group<SpotLightComponent>(entt::get<TransformComponent>);
			s_SceneRendererData.SpotLightsBuffer.Size = group2.size() < MAX_SPOT_LIGHTS ? group2.size() : MAX_SPOT_LIGHTS;
			int spotLightCounter = 0;
			for (auto entity : group2)
			{
				auto [transform, spotLight] = group2.get<TransformComponent, SpotLightComponent>(entity);

				glm::vec3 lightDirection = transform.GetDirection();
				glm::vec3 pos = transform.Translation;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].LightDir = lightDirection;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].Color = spotLight.Color;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].Cutoff = cos(glm::radians(spotLight.Cutoff));
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].OuterCutoff = cos(glm::radians(spotLight.OuterCutoff));
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].Position = pos;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].NearDist = spotLight.NearDist;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].FarDist = spotLight.FarDist;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].Kq = spotLight.Kq;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].Kl = spotLight.Kl;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].FarDist = spotLight.FarDist;
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].Bias = spotLight.Bias;



				glm::mat4 projection = glm::perspective(glm::radians(spotLight.OuterCutoff * 2), 1.0f, spotLight.NearDist, spotLight.FarDist);
				glm::mat4 view = glm::lookAt(pos, pos + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));

				s_SceneRendererData.LightSpaceMatricesDSBuffer.LightSpaceMatricesDirSpot[MAX_CASCADE_SIZE * MAX_DIR_LIGHTS + spotLightCounter].mat = projection * view;
			
				spotLightCounter++;
			}

			auto group3 = scene->m_Registry.group<PointLightComponent>(entt::get<TransformComponent>);
			s_SceneRendererData.PointLightsBuffer.Size = group3.size() <  MAX_POINT_LIGHTS ? group3.size() : MAX_POINT_LIGHTS;
			int pointLightCounter = 0;
			for (auto entity : group3)
			{
				auto [transform, pointLight] = group3.get<TransformComponent, PointLightComponent>(entity);

				glm::vec3 pos = transform.Translation;
				s_SceneRendererData.PointLightsBuffer.PointLights[pointLightCounter].Position = pos;
				s_SceneRendererData.PointLightsBuffer.PointLights[pointLightCounter].Color = pointLight.Color;
				s_SceneRendererData.PointLightsBuffer.PointLights[pointLightCounter].NearDist = pointLight.NearDist;
				s_SceneRendererData.PointLightsBuffer.PointLights[pointLightCounter].FarDist = pointLight.FarDist;
				s_SceneRendererData.PointLightsBuffer.PointLights[pointLightCounter].Kl = pointLight.Kl;
				s_SceneRendererData.PointLightsBuffer.PointLights[pointLightCounter].Kq = pointLight.Kq;

				glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, pointLight.NearDist, pointLight.FarDist);

				s_SceneRendererData.LightSpaceMatricesPointBuffer.LightSpaceMatricesPoint[pointLightCounter * 6 + 0].mat = projection *
					glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));

				s_SceneRendererData.LightSpaceMatricesPointBuffer.LightSpaceMatricesPoint[pointLightCounter * 6 + 1].mat = projection *
					glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));

				s_SceneRendererData.LightSpaceMatricesPointBuffer.LightSpaceMatricesPoint[pointLightCounter * 6 + 2].mat = projection *
					glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));

				s_SceneRendererData.LightSpaceMatricesPointBuffer.LightSpaceMatricesPoint[pointLightCounter * 6 + 3].mat = projection *
					glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));

				s_SceneRendererData.LightSpaceMatricesPointBuffer.LightSpaceMatricesPoint[pointLightCounter * 6 + 4].mat = projection *
					glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));

				s_SceneRendererData.LightSpaceMatricesPointBuffer.LightSpaceMatricesPoint[pointLightCounter * 6 + 5].mat = projection *
					glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));


				pointLightCounter++;
			}

		// ------------------- END FILL IN DIR LIGHT UNIFORMS --------------------- //



		glm::mat4 model = glm::mat4(1.0f);

		
		s_SceneRendererData.DirLightUniformBuffer->SetData(&s_SceneRendererData.DirLightsBuffer,
			sizeof(s_SceneRendererData.DirLightsBuffer));

		s_SceneRendererData.SpotLightUniformBuffer->SetData(&s_SceneRendererData.SpotLightsBuffer,
			sizeof(s_SceneRendererData.SpotLightsBuffer));

		s_SceneRendererData.PointLightUniformBuffer->SetData(&s_SceneRendererData.PointLightsBuffer,
			sizeof(s_SceneRendererData.PointLightsBuffer));

		s_SceneRendererData.LightSpaceMatricesDSUniformBuffer->SetData(&s_SceneRendererData.LightSpaceMatricesDSBuffer,
			sizeof(s_SceneRendererData.LightSpaceMatricesDSBuffer));

		s_SceneRendererData.LightSpaceMatricesPointUniformBuffer->SetData(&s_SceneRendererData.LightSpaceMatricesPointBuffer,
			sizeof(s_SceneRendererData.LightSpaceMatricesPointBuffer));

		s_SceneRendererData.CascadePlaneDistancesUniformBuffer->SetData(&s_SceneRendererData.CascadePlaneDistancesBuffer,
			sizeof(s_SceneRendererData.CascadePlaneDistancesBuffer));

		s_SceneRendererData.depthRenderPass->InvokeCommands(
			[&] () -> void {

				s_SceneRendererData.depthShader->Bind();

				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				glCullFace(GL_FRONT);
				// ------------ DRAW SCENE ------------

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
				model = glm::scale(model, glm::vec3(500.0f, 1.0f, 500.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-50.0f, 50.0f, 2.0f));
				model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.cube->Draw();


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-10.0f, 20.0f, 2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				//s_SceneRendererData.BoneMatricesUniformBuffer->SetData(&s_SceneRendererData.animatedModel->GetFinalBoneMatrices(), sizeof(SceneRendererData::BoneMatricesData));
				s_SceneRendererData.animatedModel->Draw();
				
				// ---------- DRAW SCENE END ----------
				glCullFace(GL_BACK);

			}
		);


		s_SceneRendererData.pointLightDepthRenderPass->InvokeCommands(
			[&]() -> void {

				s_SceneRendererData.pointLightDepthShader->Bind();

				glClear(GL_DEPTH_BUFFER_BIT);
				glCullFace(GL_FRONT);
				// ------------ DRAW SCENE ------------


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
				model = glm::scale(model, glm::vec3(500.0f, 1.0f, 500.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-50.0f, 50.0f, 2.0f));
				model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-10.0f, 20.0f, 2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				//s_SceneRendererData.BoneMatricesUniformBuffer->SetData(&s_SceneRendererData.animatedModel->GetFinalBoneMatrices(), sizeof(SceneRendererData::BoneMatricesData));
				s_SceneRendererData.animatedModel->Draw();

				// ---------- DRAW SCENE END ----------
				glCullFace(GL_BACK);

			}
		);

		// RENDER PASS FOR BLURING directional and spot light shadow maps altogether
		float sW = s_SceneRendererData.ShadowMapSettingsBuffer.shadowMapResX;
		float sH = s_SceneRendererData.ShadowMapSettingsBuffer.shadowMapResY;
		glm::vec2 blurScale = s_SceneRendererData.ShadowMapSettingsBuffer.blurScale;
		float firstTime = true;
		glDisable(GL_DEPTH_TEST);
		for (int i = 0; i < 2; i++)
		{
			s_SceneRendererData.depthBlurDSLRenderPass->InvokeCommandsPP(
				[&]()-> void {
					s_SceneRendererData.shadowMapDirSpotBlur->Bind();
					s_SceneRendererData.ShadowMapSettingsBuffer.blurScale = glm::vec2((1 / sW) * 1.0, 0.0);
					s_SceneRendererData.ShadowMapSettingsUniformBuffer->SetData(&s_SceneRendererData.ShadowMapSettingsBuffer, sizeof(SceneRendererData::ShadowMapSettings));
					// reset viewport
					uint32_t depthMap = 0;
					if(firstTime)
						depthMap = RenderPass::GetColorInput(s_SceneRendererData.depthRenderPass, 0);
					else
						depthMap = PingPongRenderPass::GetColorInputPP(s_SceneRendererData.depthBlurDSLRenderPass, 0);
					glBindTextureUnit(0, depthMap);
					// ------------ DRAW SCENE ------------
					s_SceneRendererData.plane->Draw();
					// ---------- DRAW SCENE END ----------
				},
				[&]()-> void {
					s_SceneRendererData.shadowMapDirSpotBlur->Bind();
					s_SceneRendererData.ShadowMapSettingsBuffer.blurScale = glm::vec2(0.0, (1 / sH) * 1.0);
					s_SceneRendererData.ShadowMapSettingsUniformBuffer->SetData(&s_SceneRendererData.ShadowMapSettingsBuffer, sizeof(SceneRendererData::ShadowMapSettings));
					uint32_t depthMap = PingPongRenderPass::GetColorInput(s_SceneRendererData.depthBlurDSLRenderPass, 0);
					glBindTextureUnit(0, depthMap);
					// ------------ DRAW SCENE ------------
					s_SceneRendererData.plane->Draw();
					// ----------- DRAW SCENE END ---------
				}
				);
		}
		glEnable(GL_DEPTH_TEST);

		// POINT LIGHT ICIN PCF UYGULA SADECE!!!!!
		
		// FINAL RENDERING - (FOR NOW!)
		
		s_SceneRendererData.finalRenderPass->InvokeCommands(
			[&]()-> void {

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				s_SceneRendererData.mainShader->Bind();

				s_SceneRendererData.WhiteTexture->Bind(0);

				uint32_t depthMap = s_SceneRendererData.depthBlurDSLRenderPass->GetColorAttachmentPP(0);
				glBindTextureUnit(0, depthMap);
				
				uint32_t depthMap2 = s_SceneRendererData.pointLightDepthRenderPass->GetDepthAttachment(0);
				glBindTextureUnit(1, depthMap2);
				// ------------ DRAW SCENE ------------

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
				model = glm::scale(model, glm::vec3(500.0f, 1.0f, 500.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				// bright grey
				s_SceneRendererData.MaterialBuffer.Color = glm::vec3(0.9f, 0.9f, 0.9f);
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.cube->Draw();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-50.0f, 50.0f, 2.0f));
				model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				// dark orange
				s_SceneRendererData.MaterialBuffer.Color = glm::vec3(0.8f, 0.2f, 0.4f);
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.cube->Draw();


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-10.0f, 20.0f, 2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				//s_SceneRendererData.BoneMatricesUniformBuffer->SetData(&s_SceneRendererData.animatedModel->GetFinalBoneMatrices(), sizeof(SceneRendererData::BoneMatricesData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.animatedModel->Draw();

				// ---------- DRAW SCENE END ----------

				s_SceneRendererData.depthDebugShader->Bind();


				glDisable(GL_DEPTH_TEST);
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));
				model = glm::scale(model, glm::vec3(0.25f, 0.25f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.plane->Draw();
				glEnable(GL_DEPTH_TEST);
				// ---------- DRAW SCENE END ----------
			}
		);

		s_SceneRendererData.postProcessingPass->InvokeCommands(
			[&]()-> void {

				s_SceneRendererData.finalRenderPass->GetFramebuffer()->BindRead();
				s_SceneRendererData.sampleResolveFramebuffer->BindDraw();
				glBlitFramebuffer(0, 0, s_SceneRendererData.ViewportSize.x, s_SceneRendererData.ViewportSize.y,
					0, 0, s_SceneRendererData.ViewportSize.x, s_SceneRendererData.ViewportSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

				s_SceneRendererData.postProcessingPass->GetFramebuffer()->Bind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				s_SceneRendererData.postProcessingShader->Bind();

				uint32_t resolvedImage = s_SceneRendererData.sampleResolveFramebuffer->GetColorAttachmentRendererID(0);
				glBindTextureUnit(0, resolvedImage);

				model = glm::mat4(1.0f);
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.plane->Draw();
			}
		);

	}

	SceneRendererStats SceneRenderer::GetStats()
	{
		return SceneRendererStats();
	}

}