#pragma once

#include <vulkan/vulkan.hpp>
#include <Platform/Vulkan/VulkanTextureUtils.h>

namespace OP
{

	struct TextureSpecification
	{
		TextureSpecification() = default;
		TextureSpecification(AttachmentFormat format) : TextureFormat(format) {}

		AttachmentFormat TextureFormat = AttachmentFormat::None;
		AttachmentSample Samples = AttachmentSample::SAMP1;

		// We might have this texture as resolve attachment for multisampled textures
		ResolveAttachment ResAttachment = ResolveAttachment::None;

	};

	struct AttachmentSpecification
	{
		AttachmentSpecification() = default;
		AttachmentSpecification(std::initializer_list<TextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<TextureSpecification> Attachments;
	};

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(const AttachmentSpecification& attachments);
		~VulkanRenderPass();

		
		VkRenderPass GetVkRenderPass();

		const std::vector<TextureSpecification>& GetColorAttachments();
		const TextureSpecification& GetDepthAttachment();

	private:
		void PopulateSpecifications(const AttachmentSpecification& attachments);
	private:
		VkRenderPass m_RenderPass;

		std::vector<TextureSpecification> m_ColorAttachmentSpecifications;
		TextureSpecification m_DepthAttachmentSpecification = AttachmentFormat::None;

		std::vector<VkAttachmentDescription> m_ColorAttachmentDescriptions;
		std::vector<VkAttachmentReference> m_ColorAttachmentReferences;

		VkAttachmentDescription m_DepthAttachmentDescription;
		VkAttachmentReference m_DepthAttachmentReference;
		VkAttachmentReference m_ResolveColorAttachmentReference;
		bool m_ResolveColorAttachmentExists = false;

	};
}