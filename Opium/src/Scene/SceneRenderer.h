#pragma once

#include <cstdint>


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

		static void Render();

		static SceneRendererStats GetStats();

	private:

	};
}