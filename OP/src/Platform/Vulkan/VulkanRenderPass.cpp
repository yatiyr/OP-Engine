#include <Precomp.h>
#include <Platform/Vulkan/VulkanRenderPass.h>
#include <Platform/Vulkan/VulkanContext.h>


namespace OP
{

	VulkanRenderPass::VulkanRenderPass(const AttachmentSpecification& attachments)
	{
		VulkanContext* context = VulkanContext::GetContext();

		PopulateSpecifications(attachments);

		// Process color attachments
		for (auto& spec : m_ColorAttachmentSpecifications)
		{
			VkAttachmentDescription colorAttachment{};
			colorAttachment.format         = GiveVkFormat(spec.TextureFormat);
			colorAttachment.samples        = GiveSampleCount(spec.Samples);
			colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			m_ColorAttachmentDescriptions.push_back(colorAttachment);

			VkAttachmentReference colorAttachmentRef{};
			colorAttachmentRef.attachment = m_ColorAttachmentDescriptions.size() - 1;
			colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			m_ColorAttachmentReferences.push_back(colorAttachmentRef);
		}

		
		if (m_DepthAttachmentSpecification.TextureFormat != AttachmentFormat::None)
		{
			m_DepthAttachmentDescription.format         = GiveVkFormat(m_DepthAttachmentSpecification.TextureFormat);
			m_DepthAttachmentDescription.samples        = GiveSampleCount(m_DepthAttachmentSpecification.Samples);
			m_DepthAttachmentDescription.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
			m_DepthAttachmentDescription.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			m_DepthAttachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			m_DepthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			m_DepthAttachmentDescription.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
			m_DepthAttachmentDescription.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			m_DepthAttachmentReference.attachment = m_ColorAttachmentDescriptions.size();
			m_DepthAttachmentReference.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			
		}

		// Subpass
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount    = m_ColorAttachmentDescriptions.size();
		subpass.pColorAttachments       = m_ColorAttachmentReferences.data();
		subpass.pDepthStencilAttachment = &m_DepthAttachmentReference;


		// Subpass Dependency
		VkSubpassDependency dependency{};
		if (m_DepthAttachmentSpecification.TextureFormat != AttachmentFormat::None)
		{
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
									  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
									  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
									   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		else
		{
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		
		std::vector<VkAttachmentDescription> allAttachments;
		for (auto& attachment : m_ColorAttachmentDescriptions)
			allAttachments.push_back(attachment);

		allAttachments.push_back(m_DepthAttachmentDescription);

		// Render Pass
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = allAttachments.size();
		renderPassInfo.pAttachments = allAttachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(context->GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to create render pass!");
		}
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		vkDestroyRenderPass(VulkanContext::GetContext()->GetDevice(), m_RenderPass, nullptr);
	}

	VkRenderPass VulkanRenderPass::GetVkRenderPass()
	{
		return m_RenderPass;
	}

	const std::vector<TextureSpecification>& VulkanRenderPass::GetColorAttachments()
	{
		return m_ColorAttachmentSpecifications;
	}

	const TextureSpecification& VulkanRenderPass::GetDepthAttachment()
	{
		return m_DepthAttachmentSpecification;
	}

	void VulkanRenderPass::PopulateSpecifications(const AttachmentSpecification& attachments)
	{
		for (auto attachment : attachments.Attachments)
		{
			if (IsDepth(attachment.TextureFormat))
			{
				m_DepthAttachmentSpecification = attachment;
			}
			else
			{
				m_ColorAttachmentSpecifications.push_back(attachment);
			}
		}
	}

}