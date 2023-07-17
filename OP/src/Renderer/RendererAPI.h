#pragma once

#include <glm/glm.hpp>
#include <Renderer/VertexArray.h>

namespace OP
{

	enum class FACE
	{
		FRONT,
		BACK,
		FRONT_AND_BACK
	};

	enum class POLYGONMODE
	{
		POINT,
		LINE,
		FILL
	};

	enum class MODE
	{
		TEXTURE_CUBE_MAP_SEAMLESS,
		DITHER,
		DEPTH_TEST,
		CULL_FACE
	};

	enum class DEPTHFUNC
	{
		NEVER,
		LESS,
		LEQUAL,
		EQUAL,
		GREATER,
		NOTEQUAL,
		GEQUAL,
		ALWAYS
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Vulkan = 2
		};
	public:
		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexedBinded(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;

		virtual void Enable(MODE mode) = 0;
		virtual void Disable(MODE mode) = 0;

		virtual void DepthFunc(DEPTHFUNC func) = 0;

		virtual void PolygonMode(FACE face, POLYGONMODE mode) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}