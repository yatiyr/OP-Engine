#pragma once

#include <vulkan/vulkan.hpp>

#include <Platform/Vulkan/VulkanContext.h>
#include <Platform/Vulkan/VulkanRenderPass.h>

namespace OP
{
	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(Ref<VulkanRenderPass> renderpass, VkImageView* attachments, uint32_t width, uint32_t height);
		~VulkanFramebuffer();

		VkFramebuffer GetVkFramebuffer();
	private:
		VkFramebuffer m_Framebuffer;
	};
}