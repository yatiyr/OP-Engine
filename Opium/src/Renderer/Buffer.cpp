#include <Precomp.h>
#include <Renderer/Buffer.h>

#include <Renderer/Renderer.h>
#include <Platform/OpenGL/OpenGLBuffer.h>

namespace Opium
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:   OP_ENGINE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}

		OP_ENGINE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:   OP_ENGINE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}

		OP_ENGINE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}