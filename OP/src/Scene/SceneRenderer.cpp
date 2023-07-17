#include <Precomp.h>
#include "SceneRenderer.h"

#include <Math/Math.h>

#include <Op/Application.h>
#include <Op/ResourceManager.h>

#include <Renderer/VertexArray.h>
#include <Renderer/Shader.h>
#include <Renderer/UniformBuffer.h>
#include <Renderer/RenderCommand.h>
#include <Renderer/PingPongRenderPass.h>

#include <Renderer/Texture.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Temp

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

#include <Profiling/Timer.h>

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
				float Near;
				float Far;
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
				float shadowMapResX = 128.0f;
				float shadowMapResY = 128.0f;
				float pointLightSMResX = 1024.0f;
				float pointLightSMResY = 1024.0f;
				glm::vec2 blurScale = glm::vec2(0.0f);
			} ShadowMapSettingsBuffer;

			Ref<UniformBuffer> ShadowMapSettingsUniformBuffer;

			struct ToneMappingSettings
			{
				float exposure = 2.0;
				bool hdr = false;
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
		Ref<Shader> entityIDShader;
		Ref<Shader> gridShader;
		Ref<Shader> outlineShader;

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
		Ref<RenderPass> entityIDFramebufferPass;


		Ref<MaterialInstance> defaultPbrMaterialInstance;

		Ref<EnvironmentMap> environmentMap;

		Timer timer;
		float tFinalRenderMiliseconds = 0.0;

		float tShadowMapDirPass = 0.0;
		float tShadowMapPointPass = 0.0;

		float tRenderPass = 0.0;
		float tPostProcessingPass = 0.0;

		float tShadowMapBlurPass = 0.0;


		bool ShowGrid = true;
	};


	static SceneRendererData s_SceneRendererData;

	void SceneRenderer::Init(float width, float height, Ref<Framebuffer> fB)
	{


	}

	void SceneRenderer::ResizeViewport(float width, float height)
	{

	}

	Ref<Framebuffer> SceneRenderer::GetFinalFramebuffer()
	{
		return s_SceneRendererData.postProcessingPass->GetFramebuffer();
	}

	Ref<Framebuffer> SceneRenderer::GetMainRenderFramebuffer()
	{
		return s_SceneRendererData.finalRenderPass->GetFramebuffer();
	}

	Ref<Framebuffer> SceneRenderer::GetEntityIDFramebuffer()
	{
		return s_SceneRendererData.entityIDFramebufferPass->GetFramebuffer();
	}

	void SceneRenderer::SetScene(Ref<Scene> scene)
	{
		s_SceneRendererData.m_Context = scene;
	}

	float* SceneRenderer::GetExposure()
	{
		return &s_SceneRendererData.ToneMappingSettingsBuffer.exposure;
	}

	void SceneRenderer::SetExposure(float Exposure)
	{
		s_SceneRendererData.ToneMappingSettingsBuffer.exposure = Exposure;
	}

	bool* SceneRenderer::GetHdr()
	{
		return &s_SceneRendererData.ToneMappingSettingsBuffer.hdr;
	}

	void SceneRenderer::SetHdr(bool Hdr)
	{
		s_SceneRendererData.ToneMappingSettingsBuffer.hdr = Hdr;
	}

	bool* SceneRenderer::GetShowGrid()
	{
		return &s_SceneRendererData.ShowGrid;
	}

	void SceneRenderer::ChangeEnvironmentMap(std::string newMapName)
	{
		/*Ref<EnvironmentMap> newMap = ResourceManager::GetEnvironmentMap(newMapName);
		if (newMap.get())
		{
			s_SceneRendererData.environmentMap->FreeMemory();
			newMap->GenerateMaps();
			s_SceneRendererData.environmentMap = newMap;
		}*/
	}

	float SceneRenderer::GetFinalRenderMiliseconds()
	{
		return s_SceneRendererData.tFinalRenderMiliseconds;
	}

	float SceneRenderer::GetShadowMapDirPassMiliseconds()
	{
		return s_SceneRendererData.tShadowMapDirPass;
	}

	float SceneRenderer::GetShadowMapPointPassMiliseconds()
	{
		return s_SceneRendererData.tShadowMapPointPass;
	}

	float SceneRenderer::GetPostProcessingPassMiliseconds()
	{
		return s_SceneRendererData.tPostProcessingPass;
	}

	float SceneRenderer::GetShadowMapBlurMiliseconds()
	{
		return s_SceneRendererData.tShadowMapBlurPass;
	}

	void SceneRenderer::RenderChain(Scene* scene, Timestep ts)
	{

	}

	void SceneRenderer::Render(SceneCamera& camera, const glm::mat4& cameraView, const glm::vec3& cameraPosition, Scene* scene, Timestep ts)
	{
	}

	void SceneRenderer::Render(EditorCamera& camera, Scene* scene, Timestep ts)
	{


	}

	SceneRendererStats SceneRenderer::GetStats()
	{
		return SceneRendererStats();
	}

}