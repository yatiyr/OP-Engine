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
#include <Geometry/Skybox.h>

#include <Utils/CSMHelpers.h>

#define MAX_DIR_LIGHTS 2
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADE_SIZE 15
#define MAX_POINT_LIGHTS 10
#define MAX_BONES 100

#include <Renderer/EnvironmentMap.h>

namespace OP
{

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
				glm::mat4 Projection;
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
				float shadowMapResX = 256.0f;
				float shadowMapResY = 256.0f;
				float pointLightSMResX = 512.0f;
				float pointLightSMResY = 512.0f;
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

		// Needs an overhaul
		struct MaterialData
		{
			glm::vec3 Color;
		} MaterialBuffer;
		
		Ref<UniformBuffer> MaterialUniformBuffer;


		// CubemapTexture
		Ref<Texture> cubemap;
		
		// Shaders
		Ref<Shader> mainShader;
		Ref<Shader> mainShaderAnimated;
		Ref<Shader> depthShader;
		Ref<Shader> depthShaderAnimated;
		Ref<Shader> pointLightDepthShader;
		Ref<Shader> pointLightDepthShaderAnimated;
		Ref<Shader> depthDebugShader;
		Ref<Shader> shadowMapDirSpotBlur;
		Ref<Shader> pointLightSMBlurShader;
		Ref<Shader> postProcessingShader;



		Ref<Cube> cube;
		Ref<Plane> plane;
		Ref<Quad> quad;
		Ref<Skybox> skybox;
		Ref<Model> animatedModel;


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



		Ref<MaterialInstance> defaultPbrMaterialInstance;





		Ref<EnvironmentMap> environmentMap;

	};


	static SceneRendererData s_SceneRendererData;

	void SceneRenderer::Init(float width, float height, Ref<Framebuffer> fB)
	{

		/*/EnvironmentMapSpec eMSpec;
		eMSpec.EquirectangularTex = ResourceManager::GetHdrTexture("neon_photostudio_4k");
		eMSpec.CubemapCaptureShader = ResourceManager::GetShader("EquirectangularToCubemap.glsl");
		eMSpec.IrradianceMapGenerationShader = ResourceManager::GetShader("CubemapConvolution.glsl");
		eMSpec.PrefilterGenerationShader = ResourceManager::GetShader("PbrPreFilter.glsl");
		eMSpec.BrdfLUTGenerationShader = ResourceManager::GetShader("BrdfLUT.glsl");
		eMSpec.SkyboxShader = ResourceManager::GetShader("SimpleSkybox.glsl"); */

		s_SceneRendererData.environmentMap = ResourceManager::GetEnvironmentMap("belfast_sunset_4k");
		s_SceneRendererData.environmentMap->GenerateMaps();


		//s_SceneRendererData.environmentMap->FreeMemory();

		//s_SceneRendererData.environmentMap = ResourceManager::GetEnvironmentMap("snow_field_4k");
		//s_SceneRendererData.environmentMap->GenerateMaps();
		
		s_SceneRendererData.skybox = Skybox::Create();
		s_SceneRendererData.plane = Plane::Create();
		s_SceneRendererData.quad = Quad::Create();




		RenderCommand::Enable(MODE::DEPTH_TEST);
		RenderCommand::Enable(MODE::CULL_FACE);
		glCullFace(GL_BACK);

		s_SceneRendererData.cube = Cube::Create();
		s_SceneRendererData.plane = Plane::Create();
		s_SceneRendererData.quad = Quad::Create();

		s_SceneRendererData.shadowMapDirSpotBlur = ResourceManager::GetShader("DirSpotShadowMappingBlur.glsl");
		s_SceneRendererData.mainShader = ResourceManager::GetShader("Main.glsl");
		s_SceneRendererData.mainShaderAnimated = ResourceManager::GetShader("MainAnimated.glsl");
		s_SceneRendererData.depthShader = ResourceManager::GetShader("DirSpotShadowMapping.glsl");
		s_SceneRendererData.depthShaderAnimated = ResourceManager::GetShader("DirSpotShadowMappingAnimated.glsl");
		s_SceneRendererData.depthDebugShader = ResourceManager::GetShader("DepthDebug.glsl");
		s_SceneRendererData.pointLightDepthShader = ResourceManager::GetShader("PointShadowMapping.glsl");
		s_SceneRendererData.pointLightDepthShaderAnimated = ResourceManager::GetShader("PointShadowMappingAnimated.glsl");
		s_SceneRendererData.postProcessingShader = ResourceManager::GetShader("PostProcessing.glsl");
		

		s_SceneRendererData.animatedModel = ResourceManager::GetModel("Swing Dancing");

		s_SceneRendererData.defaultPbrMaterialInstance = MaterialInstance::Create(ResourceManager::GetMaterial("DefaultPbr"));

		// Deal with uniform buffers
		s_SceneRendererData.CameraUniformBuffer            = UniformBuffer::Create(sizeof(SceneRendererData::CameraData), 1);
		s_SceneRendererData.TransformUniformBuffer         = UniformBuffer::Create(sizeof(SceneRendererData::TransformData), 2);
		s_SceneRendererData.ShadowMapSettingsUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::ShadowMapSettings), 3);

		s_SceneRendererData.DirLightUniformBuffer   = UniformBuffer::Create(sizeof(SceneRendererData::DirLightData), 4);
		s_SceneRendererData.SpotLightUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::SpotLightData), 5);
		s_SceneRendererData.PointLightUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::PointLightData), 6);

		s_SceneRendererData.LightSpaceMatricesDSUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::LightSpaceMatricesDSData), 7);
		s_SceneRendererData.LightSpaceMatricesPointUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::LightSpaceMatricesPointData), 8);
		s_SceneRendererData.CascadePlaneDistancesUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::CascadePlaneDistancesData), 9);

		s_SceneRendererData.MaterialUniformBuffer  = UniformBuffer::Create(sizeof(SceneRendererData::MaterialData), 10);

		s_SceneRendererData.ToneMappingSettingsUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::ToneMappingSettings), 11);


		s_SceneRendererData.BoneMatricesUniformBuffer = UniformBuffer::Create(sizeof(SceneRendererData::BoneMatricesData), 12);

		s_SceneRendererData.ViewportSize.x = width;
		s_SceneRendererData.ViewportSize.y = height;


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


		FramebufferSpecification depthFBBlurSpec;
		depthFBSpec.Attachments = { FramebufferTextureFormat::SM_VARIANCE32F, FramebufferTextureFormat::SHADOWMAP_ARRAY_DEPTH };
		depthFBSpec.Width = s_SceneRendererData.ShadowMapSettingsBuffer.shadowMapResX;
		depthFBSpec.Height = s_SceneRendererData.ShadowMapSettingsBuffer.shadowMapResY;
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
		finalFBSpec.Samples = 8;
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

	void SceneRenderer::ChangeEnvironmentMap(std::string newMapName)
	{
		Ref<EnvironmentMap> newMap = ResourceManager::GetEnvironmentMap(newMapName);
		if (newMap.get())
		{
			s_SceneRendererData.environmentMap->FreeMemory();
			newMap->GenerateMaps();
			s_SceneRendererData.environmentMap = newMap;
		}
	}

	void SceneRenderer::Render(EditorCamera& camera, Ref<Scene> scene, Timestep ts)
	{

		// -------------------- CALCULATE CAMERA DATA -------------------------------- //
			s_SceneRendererData.CameraBuffer.ViewProjection = camera.GetViewProjection();
			s_SceneRendererData.CameraBuffer.ViewPos = camera.GetPosition();
			s_SceneRendererData.CameraBuffer.View = camera.GetViewMatrix();
			s_SceneRendererData.CameraBuffer.Projection = camera.GetProjection();
			s_SceneRendererData.CameraUniformBuffer->SetData(&s_SceneRendererData.CameraBuffer, sizeof(SceneRendererData::CameraData));
		// ---------------------- END CALCULATE CAMERA DATA -------------------------- //


		// ---------------------- TONE MAPPING SETTINGS ------------------------------ //
			s_SceneRendererData.ToneMappingSettingsUniformBuffer->SetData(&s_SceneRendererData.ToneMappingSettingsBuffer, sizeof(SceneRendererData::ToneMappingSettings));


		// --------------------- UPDATE ANIMATIONS -------------------------------- //

			//s_SceneRendererData.animatedModel->UpdateAnimation(ts);
			//s_SceneRendererData.BoneMatricesUniformBuffer->SetData(&s_SceneRendererData.animatedModel->GetFinalBoneMatrices(), sizeof(SceneRendererData::BoneMatricesData));

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
					s_SceneRendererData.DirLightsBuffer.DirLights[dirLightCounter].Color = dirLight.Color * dirLight.Intensity;
					s_SceneRendererData.DirLightsBuffer.DirLights[dirLightCounter].CascadeSize = dirLight.CascadeSize;
					s_SceneRendererData.DirLightsBuffer.DirLights[dirLightCounter].FrustaDistFactor = dirLight.FrustaDistFactor;

					std::vector<float> cascadeLevels = Utils::DistributeShadowCascadeLevels(dirLight.CascadeSize, dirLight.FrustaDistFactor, camera.GetFarClip() / 20);
					OP_ENGINE_ASSERT(cascadeLevels <= MAX_CASCADE_SIZE);
					std::vector<glm::mat4> matrices = Utils::GetLightSpaceMatrices(cameraProjection, camera.GetViewMatrix(),
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
				s_SceneRendererData.SpotLightsBuffer.SpotLights[spotLightCounter].Color = spotLight.Color * spotLight.Intensity;
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
				s_SceneRendererData.PointLightsBuffer.PointLights[pointLightCounter].Color = pointLight.Color * pointLight.Intensity;
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

				auto meshes = scene->m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
				for (auto mesh : meshes)
				{
					auto [tC, mC] = meshes.get<TransformComponent, MeshComponent>(mesh);

					s_SceneRendererData.TransformBuffer.Model = tC.GetTransform();
					s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
					if(mC.Mesh)
						mC.Mesh->Draw();
				}


				/*s_SceneRendererData.depthShaderAnimated->Bind();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-10.0f, 20.0f, 2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				//s_SceneRendererData.BoneMatricesUniformBuffer->SetData(&s_SceneRendererData.animatedModel->GetFinalBoneMatrices(), sizeof(SceneRendererData::BoneMatricesData));
				s_SceneRendererData.animatedModel->Draw(); */
				
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

				auto meshes = scene->m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
				for (auto mesh : meshes)
				{
					auto [tC, mC] = meshes.get<TransformComponent, MeshComponent>(mesh);

					s_SceneRendererData.TransformBuffer.Model = tC.GetTransform();
					s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
					if (mC.Mesh)
						mC.Mesh->Draw();
				}

				/*s_SceneRendererData.pointLightDepthShaderAnimated->Bind();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-10.0f, 20.0f, 2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				//s_SceneRendererData.BoneMatricesUniformBuffer->SetData(&s_SceneRendererData.animatedModel->GetFinalBoneMatrices(), sizeof(SceneRendererData::BoneMatricesData));
				s_SceneRendererData.animatedModel->Draw(); */

				// ---------- DRAW SCENE END ----------
				glCullFace(GL_BACK);

			}
		);

		// RENDER PASS FOR BLURING directional and spot light shadow maps altogether
		float sW = s_SceneRendererData.ShadowMapSettingsBuffer.shadowMapResX;
		float sH = s_SceneRendererData.ShadowMapSettingsBuffer.shadowMapResY;
		glm::vec2 blurScale = s_SceneRendererData.ShadowMapSettingsBuffer.blurScale;
		float firstTime = true;
		RenderCommand::Disable(MODE::DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
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
		RenderCommand::Enable(MODE::DEPTH_TEST);

		// POINT LIGHT ICIN PCF UYGULA SADECE!!!!!
		
		// FINAL RENDERING - (FOR NOW!)
		
		s_SceneRendererData.finalRenderPass->InvokeCommands(
			[&]()-> void {

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


				s_SceneRendererData.mainShader->Bind();

				uint32_t depthMap = s_SceneRendererData.depthBlurDSLRenderPass->GetColorAttachmentPP(0);
				glBindTextureUnit(0, depthMap);
				
				uint32_t depthMap2 = s_SceneRendererData.pointLightDepthRenderPass->GetDepthAttachment(0);
				glBindTextureUnit(1, depthMap2);
				// ------------ DRAW SCENE ------------

				/*uint32_t environmentMap = s_SceneRendererData.irradianceMapGenerationRenderPass->GetColorAttachment(0);
				glBindTextureUnit(2, environmentMap);

				uint32_t prefilterMap = s_SceneRendererData.prefilterGenerationRenderPass->GetColorAttachment(0);
				glBindTextureUnit(3, prefilterMap);

				uint32_t brdfLUT = s_SceneRendererData.brdfLUTGenerationPass->GetColorAttachment(0);
				glBindTextureUnit(4, brdfLUT);*/

				s_SceneRendererData.environmentMap->BindIrradianceMap(2);
				s_SceneRendererData.environmentMap->BindPrefilterMap(3);
				s_SceneRendererData.environmentMap->BindBrdfLUT(4);

				uint32_t ditheringTex = ResourceManager::GetTexture("BayerMatrixDithering")->GetRendererID();
				glBindTextureUnit(5, ditheringTex);

				auto meshes = scene->m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
				for (auto mesh : meshes)
				{
					auto [tC, mC] = meshes.get<TransformComponent, MeshComponent>(mesh);

					bool hasMaterial = scene->m_Registry.any_of<MaterialComponent>(mesh);

					s_SceneRendererData.TransformBuffer.Model = tC.GetTransform();
					s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));

					if (!hasMaterial)
					{
						s_SceneRendererData.defaultPbrMaterialInstance->Mat->Bind();
						glBindTextureUnit(0, depthMap);
						glBindTextureUnit(1, depthMap2);
						s_SceneRendererData.defaultPbrMaterialInstance->AssignValues();
					}
					else
					{
						auto matC = scene->m_Registry.get<MaterialComponent>(mesh);
						matC.MatInstance->Mat->Bind();
						glBindTextureUnit(0, depthMap);
						glBindTextureUnit(1, depthMap2);
						matC.MatInstance->AssignValues();
					}
					if (mC.Mesh)
						mC.Mesh->Draw();


					/*s_SceneRendererData.MaterialBuffer.Color = glm::vec3(0.9f, 0.3f, 0.2f);
					s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));*/
				}

				s_SceneRendererData.environmentMap->RenderSkybox();

				/*RenderCommand::DepthFunc(DEPTHFUNC::LEQUAL);
				// Render Skybox last
				glCullFace(GL_FRONT);
				s_SceneRendererData.skyboxShader->Bind();

				//environmentMap = s_SceneRendererData.cubemapCaptureRenderPass->GetColorAttachment(0);
				//glBindTextureUnit(0, environmentMap);
				s_SceneRendererData.environmentMap->BindEnvironmentCubemap(0);

				s_SceneRendererData.skybox->Draw();
				glCullFace(GL_BACK);
				RenderCommand::DepthFunc(DEPTHFUNC::LESS); */
				/*s_SceneRendererData.equirectangularToCubeShader->Bind();
				glBindTextureUnit(1, s_SceneRendererData.cubemap->GetRendererID());
				s_SceneRendererData.cube->Draw();*/

				/*s_SceneRendererData.mainShaderAnimated->Bind();

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-10.0f, 20.0f, 2.0f));
				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				//s_SceneRendererData.BoneMatricesUniformBuffer->SetData(&s_SceneRendererData.animatedModel->GetFinalBoneMatrices(), sizeof(SceneRendererData::BoneMatricesData));
				s_SceneRendererData.MaterialUniformBuffer->SetData(&s_SceneRendererData.MaterialBuffer, sizeof(SceneRendererData::MaterialData));
				s_SceneRendererData.animatedModel->Draw(); */

				// ---------- DRAW SCENE END ----------

				/*s_SceneRendererData.depthDebugShader->Bind();
				uint32_t depthMapDbg = s_SceneRendererData.brdfLUTGenerationPass->GetColorAttachment(0);

				glBindTextureUnit(1, depthMapDbg);
				glDisable(GL_DEPTH_TEST);
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));
				model = glm::scale(model, glm::vec3(0.25f, 0.25f, 1.0f));
				s_SceneRendererData.TransformBuffer.Model = model;
				s_SceneRendererData.TransformUniformBuffer->SetData(&s_SceneRendererData.TransformBuffer, sizeof(SceneRendererData::TransformData));
				s_SceneRendererData.plane->Draw();
				glEnable(GL_DEPTH_TEST); */
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