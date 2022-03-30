#include <Precomp.h>
#include <Platform/OpenGL/OpenGLContext.h>


#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace OP
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		OP_ENGINE_ASSERT(windowHandle, "Window Handle is not initialized")
	}

	void OpenGLContext::Init()
	{
		OP_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		OP_ENGINE_ASSERT(status, "Could not initialize GLAD properly!");

		OP_ENGINE_INFO("Information about OPENGL Renderer and Graphics Card");
		OP_ENGINE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		OP_ENGINE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		OP_ENGINE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		OP_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}