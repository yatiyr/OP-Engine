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
		uint32_t Samples = 1;
		uint32_t layerCount = MAX_DIR_LIGHTS * MAX_CASCADE_SIZE + MAX_SPOT_LIGHTS;
		uint32_t pointLightLayerCount = MAX_POINT_LIGHTS * 6;
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

	};
}