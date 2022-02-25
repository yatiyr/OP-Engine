#include <Precomp.h>
#include <Renderer/VertexArray.h>
#include <Renderer/Renderer.h>

#include <Platform/OpenGL/OpenGLVertexArray.h>

namespace Opium
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:   OP_ENGINE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}

		OP_ENGINE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}