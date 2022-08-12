#include <Precomp.h>
#include <Platform/OpenGL/OpenGLRendererAPI.h>

#include <glad/glad.h>

namespace OP
{

	static int OPModeToGL(MODE mode)
	{
		switch (mode)
		{
			case OP::MODE::TEXTURE_CUBE_MAP_SEAMLESS:
				return GL_TEXTURE_CUBE_MAP_SEAMLESS;
			case OP::MODE::DITHER:
				return GL_DITHER;
			case OP::MODE::DEPTH_TEST:
				return GL_DEPTH_TEST;
			case OP::MODE::CULL_FACE:
				return GL_CULL_FACE;
			default:
				return -1;
		}
	}

	static int OPDepthFuncToGL(DEPTHFUNC func)
	{
		switch (func)
		{
			case OP::DEPTHFUNC::NEVER:
				return GL_NEVER;
			case OP::DEPTHFUNC::LESS:
				return GL_LESS;
			case OP::DEPTHFUNC::LEQUAL:
				return GL_LEQUAL;
			case OP::DEPTHFUNC::EQUAL:
				return GL_EQUAL;
			case OP::DEPTHFUNC::GREATER:
				return GL_GREATER;
			case OP::DEPTHFUNC::NOTEQUAL:
				return GL_NOTEQUAL;
			case OP::DEPTHFUNC::GEQUAL:
				return GL_GEQUAL;
			case OP::DEPTHFUNC::ALWAYS:
				return GL_ALWAYS;
			default:
				break;
		}
	}

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{

		// I could not understand why cherno did the thing below, it created some artifacts in
		// my case. Maybe I did wrong while I was following him. 
		// uint32_t count = indexCount ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;

		// I'm giving indexCount, it should be enough I guess
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexedBinded(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		vertexArray->Unbind();
	}

	void OpenGLRendererAPI::Enable(MODE mode)
	{
		glEnable(OPModeToGL(mode));
	}

	void OpenGLRendererAPI::Disable(MODE mode)
	{
		glDisable(OPModeToGL(mode));
	}

	void OpenGLRendererAPI::DepthFunc(DEPTHFUNC func)
	{
		glDepthFunc(OPDepthFuncToGL(func));
	}

}