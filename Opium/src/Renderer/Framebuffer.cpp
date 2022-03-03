#include <Precomp.h>
#include <Renderer/Framebuffer.h>
#include <Platform/OpenGL/OpenGLFramebuffer.h>
#include <Renderer/Renderer.h>

namespace Opium
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:   OP_ENGINE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLFramebuffer>(spec);
		}

		OP_ENGINE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}