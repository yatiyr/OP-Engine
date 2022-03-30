#include <Precomp.h>
#include <Renderer/RenderCommand.h>

#include <Platform/OpenGL/OpenGLRendererAPI.h>

namespace OP
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}