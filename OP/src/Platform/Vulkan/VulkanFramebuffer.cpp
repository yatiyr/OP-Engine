#include <Precomp.h>
#include <Platform/Vulkan/VulkanContext.h>

#include <Platform/Vulkan/VulkanFramebuffer.h>
#include <Platform/Vulkan/VulkanBufferUtils.h>
#include <Platform/Vulkan/VulkanUtils.h>
#include <Platform/Vulkan/VulkanTextureUtils.h>

namespace OP
{

	VulkanFramebuffer::VulkanFramebuffer(Ref<VulkanRenderPass> renderpass, uint32_t width, uint32_t height)
	{
		std::vector<TextureSpecification> colorAttachments = renderpass->GetColorAttachments();
		TextureSpecification depthAttachment = renderpass->GetDepthAttachment();


	}

	// This constructor is currently used for creating the swapchain framebuffer
	// I will delete the depth attachment later!
	VulkanFramebuffer::VulkanFramebuffer(Ref<VulkanRenderPass> renderpass, VkImageView swapchainAttachment, uint32_t width, uint32_t height)
	{
		VulkanContext* context = VulkanContext::GetContext();
		VkDevice device = context->GetDevice();		
		VkPhysicalDevice physicalDevice = context->GetPhysicalDevice();

		VkSampleCountFlagBits maxSampleCount = context->GetMaxSampleCount();

		TextureSpecification depthAttachment = renderpass->GetDepthAttachment();

		if (depthAttachment.TextureFormat != AttachmentFormat::None)
		{
			VkFormat texFormat = TextureUtils::GiveVkFormat(depthAttachment.TextureFormat);
			VkSampleCountFlagBits candidateSampleCount = TextureUtils::GiveSampleCount(depthAttachment.Samples);

			TextureUtils::CreateImage(device,
				                      physicalDevice,
				                      width,
									  height,
									  1,
									  maxSampleCount < candidateSampleCount ? maxSampleCount : candidateSampleCount,
									  texFormat,
									  VK_IMAGE_TILING_OPTIMAL,
									  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
									  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
									  m_DepthImage,
									  m_DepthImageMemory);

			m_DepthImageView = TextureUtils::CreateImageView(device,
				                                             m_DepthImage,
				                                             texFormat,
				                                             VK_IMAGE_ASPECT_DEPTH_BIT, 1);
		}

		for (auto& att : renderpass->GetColorAttachments())
		{
			if (att.ResAttachment != ResolveAttachment::None)
				continue;

			VkFormat texFormat = TextureUtils::GiveVkFormat(att.TextureFormat);
			VkSampleCountFlagBits candidateSampleCount = TextureUtils::GiveSampleCount(att.Samples);

			m_ColorImages.resize(1);
			m_ColorImageMemories.resize(1);
			m_ColorImageViews.resize(1);

			TextureUtils::CreateImage(device,
									  physicalDevice,
									  width,
									  height,
									  1,
								      maxSampleCount < candidateSampleCount ? maxSampleCount : candidateSampleCount,
									  texFormat,
									  VK_IMAGE_TILING_OPTIMAL,
									  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
									  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
									  m_ColorImages[0],
									  m_ColorImageMemories[0]);

			m_ColorImageViews[0] = TextureUtils::CreateImageView(device, m_ColorImages[0], texFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}

		std::vector<VkImageView> atts;
		atts.push_back(m_ColorImageViews[0]);
		atts.push_back(swapchainAttachment);
		atts.push_back(m_DepthImageView);

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass->GetVkRenderPass();
		// TODO: THESE WILL BE CHANGED
		framebufferInfo.attachmentCount = atts.size();
		framebufferInfo.pAttachments = atts.data();
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
		CleanupFramebuffer();
	}

	VkFramebuffer VulkanFramebuffer::GetVkFramebuffer()
	{
		return m_Framebuffer;
	}

	void VulkanFramebuffer::ResizeFramebuffer(VkImageView swapchainAttachment, uint32_t width, uint32_t height)
	{
	}

	void VulkanFramebuffer::CleanupFramebuffer()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();

		for (auto& imageView : m_ColorImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		for (auto& image : m_ColorImages)
		{
			vkDestroyImage(device, image, nullptr);
		}

		vkDestroyImageView(device, m_DepthImageView, nullptr);
		vkDestroyImage(device, m_DepthImage, nullptr);

		vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
	}

}