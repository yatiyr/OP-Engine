#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Renderer/GraphicsContext.h>

struct GLFWwindow;

namespace OP
{
	class VulkanContext : public GraphicsContext
	{
	public:

		VulkanContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void Cleanup() override;
	private:
		void CreateInstance();
		bool checkValidationLayerSupport();
	private:
		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance;
	};
}