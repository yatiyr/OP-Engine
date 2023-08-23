#include <Precomp.h>
#include <Platform/Vulkan/VulkanFramebuffer.h>

#include <Platform/Vulkan/VulkanBufferUtils.h>
#include <Platform/Vulkan/VulkanUtils.h>

namespace OP
{

	VulkanFramebuffer::VulkanFramebuffer(Ref<VulkanRenderPass> renderpass, uint32_t width, uint32_t height)
	{
		
	}

	VulkanFramebuffer::VulkanFramebuffer(Ref<VulkanRenderPass> renderpass, VkImageView* attachments, uint32_t width, uint32_t height)
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass->GetVkRenderPass();
		// TODO: THESE WILL BE CHANGED
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_Framebuffer) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to create framebuffer!");
		}
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
	}

	VkFramebuffer VulkanFramebuffer::GetVkFramebuffer()
	{
		return m_Framebuffer;
	}

}