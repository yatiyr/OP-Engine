#include <Precomp.h>
#include <Renderer/RenderCommand.h>

#include <Platform/OpenGL/OpenGLRendererAPI.h>

namespace Opium
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}