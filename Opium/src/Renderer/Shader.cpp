#include <Precomp.h>
#include <Renderer/Shader.h>

#include <Renderer/Renderer.h>
#include <Platform/OpenGL/OpenGLShader.h>

namespace Opium
{
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:   OP_ENGINE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		OP_ENGINE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
