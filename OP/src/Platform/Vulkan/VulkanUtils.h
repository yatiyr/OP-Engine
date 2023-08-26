#pragma once

#include <vulkan/vulkan.hpp>
#include <Platform/Vulkan/VulkanContext.h>


namespace OP
{
	namespace VulkanUtils
	{

		static VkFormat FindSupportedFormat(VkDevice device, VkPhysicalDevice physicalDevice,
			                                const std::vector<VkFormat>& candidates,
			                                VkImageTiling tiling,
			                                VkFormatFeatureFlags features)
		{

			for (VkFormat format : candidates)
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

				if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				{
					return format;
				}
				else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				{
					return format;
				}

				
			}

			OP_ENGINE_ERROR("Failed to find supported format!");

		}

		static VkFormat FindDepthFormat(VkDevice device, VkPhysicalDevice physicalDevice)
		{
			return FindSupportedFormat(device, physicalDevice,
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			);
		}

		static bool HasStencilComponent(VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}




	}
}