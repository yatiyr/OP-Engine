#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Renderer/GraphicsContext.h>
struct GLFWwindow;

#include <Op/ResourceManager.h>
#include <Platform/Vulkan/VulkanRenderPass.h>
#include <Platform/Vulkan/VulkanFramebuffer.h>
#include <Platform/Vulkan/VulkanGraphicsPipeline.h>
#include <optional>
#include <vector>


#define GLM_FORCE_DEPTH_ZERO_TO_ONE

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

		virtual void SwapBuffers() override;
		virtual void Cleanup()     override;
		virtual void Init()        override;


		VkPhysicalDevice GetPhysicalDevice();
		VkExtent2D& GetSwapChainExtent();
		VkFormat& GetSwapChainImageFormat();
		VkDevice         GetDevice();


		std::vector<VkImageView>& GetSwapChainImageViews();
		VkCommandPool& GetCommandPool();

		std::vector<VkSemaphore>& GetImageAvailableSemaphores();
		std::vector<VkSemaphore>& GetRenderFinishedSemaphores();
		std::vector<VkFence>& GetInFlightFences();

		VkSwapchainKHR GetSwapchain();

		VkQueue GetGraphicsQueue();
		VkQueue GetPresentQueue();

		int GetMaxFramesInFlight();

		static VulkanContext* GetContext();

		std::vector<Ref<VulkanFramebuffer>>& GetSwapChainFramebuffers();
		Ref<VulkanRenderPass> GetSwapChainRenderPass();

		VkSampleCountFlagBits GetMaxSampleCount();
	private:
		void CreateSurface();
		void CreateInstance();
		void CreateSwapchain();
		void CreateImageViews();
		void CreateCommandPool();
		void CreateSyncObjects();
		void SetupDebugMessenger();
		void CreateLogicalDevice();
		void CreateSwapchainRenderPass();
		void CreateSwapchainFramebuffers();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void PickPhysicalDevice();

		bool checkValidationLayerSupport();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		std::vector<const char*> GetRequiredExtensions();

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);


		int RateDevice(VkPhysicalDevice device);

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);


		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


		VkSampleCountFlagBits GetMaxUsableSampleCount();

		void CleanupSwapchain();
		void CleanupSwapchainFramebuffers();
	private:
		static VulkanContext* s_Instance;
	private:
		VkInstance m_Instance;
		VkDevice m_Device;
		GLFWwindow* m_WindowHandle;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkSurfaceKHR m_Surface;
		VkCommandPool m_CommandPool;

		// Swapchain Elements
		VkSwapchainKHR m_SwapChain;
		VkExtent2D m_SwapChainExtent;
		VkFormat m_SwapChainImageFormat;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<Ref<VulkanFramebuffer>> m_SwapChainFramebuffers;
		Ref<VulkanRenderPass> m_SwapChainRenderPass;

		// Sync
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		// MSAA - We keep track of maximum samples that can be achieved
		// with the current hardware
		VkSampleCountFlagBits m_MaxMSAASamples;

		int m_MaxFramesInFlight;
		friend class VulkanRenderSystem;
	};
}