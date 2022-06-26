#pragma once

#include <cstdint>
#include <Renderer/EditorCamera.h>

namespace OP
{

	struct SceneRendererStats
	{
		uint32_t totalVertices = 0;
	};

	class SceneRenderer
	{
	public:
		static void Init();

		static void Render(EditorCamera& camera);

		static SceneRendererStats GetStats();

		static void ResizeViewport(float width, float height);

	private:

	};
}