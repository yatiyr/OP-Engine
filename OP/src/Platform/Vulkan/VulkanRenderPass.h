#pragma once

#include <vulkan/vulkan.hpp>

namespace OP
{

	enum class AttachmentFormat
	{
		None = 0,

		// Color
		RGBA8, RGBA32F, SRGB, RED_INTEGER, SM_VARIANCE32F,

		// Depth / stencil
		DEPTH24STENCIL8,
		DEPTH32,
		DEPTH32STENCIL8,

		// Default
		Depth = DEPTH24STENCIL8
	};

	VkFormat GiveVkFormat(AttachmentFormat format)
	{
		switch (format)
		{
		case OP::AttachmentFormat::None:
			return VK_FORMAT_UNDEFINED;
		case OP::AttachmentFormat::RGBA8:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case OP::AttachmentFormat::RGBA32F:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case OP::AttachmentFormat::SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case OP::AttachmentFormat::RED_INTEGER:
			return VK_FORMAT_R8_UINT;
		case OP::AttachmentFormat::SM_VARIANCE32F:
			return VK_FORMAT_D32_SFLOAT;
		case OP::AttachmentFormat::DEPTH24STENCIL8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case OP::AttachmentFormat::DEPTH32:
			return VK_FORMAT_D32_SFLOAT;
		case OP::AttachmentFormat::DEPTH32STENCIL8:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		default:
		{
			OP_ENGINE_ERROR("Could not find appropriate format!");
			return VK_FORMAT_UNDEFINED;
		}

		}
	}

	VkSampleCountFlagBits GiveSampleCount(uint32_t sampleCount)
	{
		switch (sampleCount)
		{
		case 1:
			return VK_SAMPLE_COUNT_1_BIT;
		case 2:
			return VK_SAMPLE_COUNT_2_BIT;
		case 4:
			return VK_SAMPLE_COUNT_4_BIT;
		case 8:
			return VK_SAMPLE_COUNT_8_BIT;
		case 16:
			return VK_SAMPLE_COUNT_16_BIT;
		case 32:
			return VK_SAMPLE_COUNT_32_BIT;
		case 64:
			return VK_SAMPLE_COUNT_64_BIT;
		default:
			return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
		}
	}

	bool IsDepth(AttachmentFormat format)
	{
		if (format == AttachmentFormat::DEPTH24STENCIL8 ||
			format == AttachmentFormat::DEPTH32 ||
			format == AttachmentFormat::DEPTH32STENCIL8)
			return true;

		return false;
	}

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