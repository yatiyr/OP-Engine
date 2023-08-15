#pragma once

#include <vulkan/vulkan.hpp>
#include <Platform/Vulkan/VulkanContext.h>


namespace OP
{
	namespace VulkanUtils
	{
		static VkImageView CreateImageView(VkImage image, VkFormat format)
		{
			VkDevice device = VulkanContext::GetContext()->GetDevice();

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VkImageView imageView;

			if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				OP_ENGINE_ERROR("Failed to craete texture image view!");
			}

			return imageView;
		}
	}
}