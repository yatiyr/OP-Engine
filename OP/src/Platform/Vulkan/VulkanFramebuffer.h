#pragma once

#include <vulkan/vulkan.hpp>

#include <Platform/Vulkan/VulkanContext.h>
#include <Platform/Vulkan/VulkanRenderPass.h>

namespace OP
{

	// Forward declaration of VulkanRenderPass
	class VulkanRenderPass;

	struct VulkanFramebufferSpecification
	{
		uint32_t Width, Height;

		bool SwapchainTarget = false;
	};

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(Ref<VulkanRenderPass> renderpass, uint32_t width, uint32_t height);
		VulkanFramebuffer(Ref<VulkanRenderPass> renderpass, VkImageView swapchainAttachment, uint32_t width, uint32_t height);
		~VulkanFramebuffer();

		VkFramebuffer GetVkFramebuffer();


		void ResizeFramebuffer(uint32_t width, uint32_t height);
		void ResizeFramebuffer(VkImageView swapchainAttachment, uint32_t width, uint32_t height);
	private:
		void CleanupFramebuffer();
	private:
		VkFramebuffer m_Framebuffer;

		Ref<RenderPass> m_RenderPass;

		std::vector<VkImage> m_ColorImages;
		std::vector<VkDeviceMemory> m_ColorImageMemories;
		std::vector<VkImageView> m_ColorImageViews;

		VkImage m_DepthImage;
		VkDeviceMemory m_DepthImageMemory;
		VkImageView m_DepthImageView;
	};
}