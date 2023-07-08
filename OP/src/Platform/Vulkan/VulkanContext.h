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
		void SetupDebugMessenger();
		std::vector<const char*> GetRequiredExtensions();

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
											  const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
										       const VkAllocationCallbacks* pAllocator);

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	private:
		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
	};
}