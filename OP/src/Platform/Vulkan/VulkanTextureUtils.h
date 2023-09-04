#pragma once

#include <Platform/Vulkan/VulkanContext.h>
#include <Platform/Vulkan/VulkanBufferUtils.h>
#include <Platform/Vulkan/VulkanUtils.h>
#include <vulkan/vulkan.hpp>

namespace OP
{

	enum class AttachmentFormat
	{
		None = 0,

		// Color
		BGRA8, RGBA8, RGBA32F, SRGB, RED_INTEGER, SM_VARIANCE32F,

		// Depth / stencil
		DEPTH24STENCIL8,
		DEPTH32,
		DEPTH32STENCIL8,

		// Default
		Depth = DEPTH24STENCIL8
	};

	namespace TextureUtils
	{

		static AttachmentFormat GiveAttachmentFormat(VkFormat format)
		{
			switch (format)
			{
			case VK_FORMAT_UNDEFINED:
				return OP::AttachmentFormat::None;
			case VK_FORMAT_B8G8R8A8_UNORM:
				return OP::AttachmentFormat::BGRA8;
			case VK_FORMAT_R8G8B8A8_UNORM:
				return OP::AttachmentFormat::RGBA8;
			case  VK_FORMAT_R32G32B32A32_SFLOAT:
				return OP::AttachmentFormat::RGBA32F;
			case VK_FORMAT_R8G8B8A8_SRGB:
				return OP::AttachmentFormat::SRGB;
			case VK_FORMAT_R8_UINT:
				return OP::AttachmentFormat::RED_INTEGER;
			case VK_FORMAT_D32_SFLOAT:
				return OP::AttachmentFormat::DEPTH32;
			case VK_FORMAT_D24_UNORM_S8_UINT:
				return OP::AttachmentFormat::DEPTH24STENCIL8;
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				return OP::AttachmentFormat::DEPTH32STENCIL8;
			default:
			{
				OP_ENGINE_ERROR("Could not find appropriate format!");
				return OP::AttachmentFormat::None;
			}

			}
		}

		static VkFormat GiveVkFormat(AttachmentFormat format)
		{
			switch (format)
			{
			case OP::AttachmentFormat::None:
				return VK_FORMAT_UNDEFINED;
			case OP::AttachmentFormat::BGRA8:
				return VK_FORMAT_B8G8R8A8_UNORM;
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

		static VkSampleCountFlagBits GiveSampleCount(uint32_t sampleCount)
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

		static bool IsDepth(AttachmentFormat format)
		{
			if (format == AttachmentFormat::DEPTH24STENCIL8 ||
				format == AttachmentFormat::DEPTH32 ||
				format == AttachmentFormat::DEPTH32STENCIL8)
				return true;

			return false;
		}

		static bool HasStencilComponent(AttachmentFormat format)
		{
			if (format == AttachmentFormat::DEPTH24STENCIL8 || format == AttachmentFormat::DEPTH32STENCIL8)
				return true;

			return false;
		}

		static void CreateImage(VkDevice device, VkPhysicalDevice physicalDevice,
			                    uint32_t width, uint32_t height, uint32_t mipLevels,
			                    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
								VkMemoryPropertyFlags properties, VkImage& image,
								VkDeviceMemory& imageMemory)
		{

			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = mipLevels;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = tiling;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usage;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
				OP_ENGINE_ERROR("Failed to create the image");
			}

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(device, image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = OP::BufferUtils::FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
			{
				OP_ENGINE_ERROR("Failed to allocate image memory!");
			}

			vkBindImageMemory(device, image, imageMemory, 0);
		}

		static VkImageView CreateImageView(VkDevice device,
			                               VkImage image,
			                               VkFormat format,
			                               VkImageAspectFlags aspectFlags, uint32_t mipLevels)
		{

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = format;
			viewInfo.subresourceRange.aspectMask = aspectFlags;//VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = mipLevels;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VkImageView imageView;

			if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				OP_ENGINE_ERROR("Failed to craete texture image view!");
			}

			return imageView;
		}

		static void TransitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue queue,
			                              VkImage image, VkFormat format, VkImageLayout oldLayout,
			VkImageLayout newLayout, uint32_t mipLevels)
		{
			VkCommandBuffer commandBuffer = OP::BufferUtils::BeginSingleTimeCommands(device, commandPool);

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			barrier.image = image;
			if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT)
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				else
					barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}
			else
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = mipLevels;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else
			{
				OP_ENGINE_ERROR("Unsupported layout transition!");
			}

			vkCmdPipelineBarrier(commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			OP::BufferUtils::EndSingleTimeCommands(device, commandPool, queue, commandBuffer);
		}

		static void CopyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue queue,
			                          VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
		{
			VkCommandBuffer commandBuffer = OP::BufferUtils::BeginSingleTimeCommands(device, commandPool);

			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = { width, height, 1 };

			vkCmdCopyBufferToImage(
				commandBuffer,
				buffer,
				image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&region
			);

			OP::BufferUtils::EndSingleTimeCommands(device, commandPool, queue, commandBuffer);
		}

		static void GenerateMipMaps(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue,
			VkImage image, VkFormat imageFormat, uint32_t width, uint32_t height, uint32_t mipLevels)
		{

			// Check if image format supports linear blitting
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
				OP_ENGINE_ERROR("texture image format does not support linear blitting!");
			}

			VkCommandBuffer commandBuffer = OP::BufferUtils::BeginSingleTimeCommands(device, commandPool);

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			uint32_t mipWidth  = width;
			uint32_t mipHeight = height;

			for (uint32_t i = 1; i < mipLevels; i++)
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				VkImageBlit blit{};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { static_cast<int32_t>(mipWidth), static_cast<int32_t>(mipHeight), 1 };
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? static_cast<int32_t>(mipWidth) / 2 : 1,
									   mipHeight > 1 ? static_cast<int32_t>(mipHeight) / 2 : 1, 1 };
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				vkCmdBlitImage(commandBuffer,
					image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit, VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
			}

			barrier.subresourceRange.baseMipLevel = mipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			OP::BufferUtils::EndSingleTimeCommands(device, commandPool, queue, commandBuffer);
		}
	}

}