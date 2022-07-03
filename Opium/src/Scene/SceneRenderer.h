#pragma once

#include <cstdint>
#include <Renderer/EditorCamera.h>
#include <Renderer/Framebuffer.h>

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

		static void Render(EditorCamera& camera, Timestep ts);

		static SceneRendererStats GetStats();

		static void ResizeViewport(float width, float height);

	private:

	};
}