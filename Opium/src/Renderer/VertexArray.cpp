#include <Precomp.h>
#include <Renderer/VertexArray.h>
#include <Renderer/Renderer.h>

#include <Platform/OpenGL/OpenGLVertexArray.h>

namespace Opium
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:   OP_ENGINE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
		}

		OP_ENGINE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}