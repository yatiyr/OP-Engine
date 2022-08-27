#pragma once

#include <cstdint>
#include <Renderer/EditorCamera.h>
#include <Renderer/Framebuffer.h>
#include <Scene/Scene.h>
#include <Scene/Components.h>

namespace OP
{

	struct SceneRendererStats
	{
		uint32_t totalVertices = 0;
	};

	class SceneRenderer
	{
	public:
		static void Init(float width, float height, Ref<Framebuffer> fB);

		static void Render(EditorCamera& camera, Scene* scene, Timestep ts);

		static SceneRendererStats GetStats();

		static void ResizeViewport(float width, float height);

		static Ref<Framebuffer> GetFinalFramebuffer();
		static Ref<Framebuffer> GetMainRenderFramebuffer();

		static void SetScene(Ref<Scene> scene);

		static float* GetExposure();
		static void SetExposure(float Exposure);

		static bool* GetHdr();
		static void SetHdr(bool Hdr);

		static bool* GetShowGrid();

		static void ChangeEnvironmentMap(std::string newMapName);

		static float GetFinalRenderMiliseconds();
		static float GetShadowMapDirPassMiliseconds();
		static float GetShadowMapPointPassMiliseconds();
		static float GetPostProcessingPassMiliseconds();
		static float GetShadowMapBlurMiliseconds();
	private:

	};
}