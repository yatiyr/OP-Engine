#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Renderer/GraphicsContext.h>
struct GLFWwindow;

#include <optional>
#include <vector>


#include <Platform/Vulkan/VulkanGraphicsPipeline.h>
#include <Platform/Vulkan/VulkanRenderPass.h>

#include <Op/ResourceManager.h>

namespace OP
{
	// Forward declaration
	class VulkanGraphicsPipeline;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanContext : public GraphicsContext
	{
	public:

		VulkanContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void Cleanup() override;
		static VulkanContext* GetContext();
		VkDevice GetDevice();
		VkPhysicalDevice GetPhysicalDevice();
		VkExtent2D& GetSwapChainExtent();
		VkFormat& GetSwapChainImageFormat();

		std::vector<VkImageView>& GetSwapChainImageViews();
		VkCommandPool& GetCommandPool();

		std::vector<VkSemaphore>& GetImageAvailableSemaphores();
		std::vector<VkSemaphore>& GetRenderFinishedSemaphores();
		std::vector<VkFence>& GetInFlightFences();

		VkSwapchainKHR GetSwapchain();

		VkQueue GetGraphicsQueue();
		VkQueue GetPresentQueue();

		int GetMaxFramesInFlight();
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
		void PickPhysicalDevice();
		int RateDevice(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		void CreateLogicalDevice();
		void CreateSurface();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void CreateSwapchain();
		void CreateImageViews();
		void CreateCommandPool();
		void CreateSyncObjects();

		void CleanupSwapchain();
	private:
		static VulkanContext* s_Instance;
		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;
		VkCommandPool m_CommandPool;

		// Sync
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		int m_MaxFramesInFlight;

		friend class VulkanRenderSystem;
	};
}