#pragma once

#include <Platform/Vulkan/VulkanContext.h>
#include <vulkan/vulkan.hpp>

namespace OP
{

	// Utility functions
	namespace BufferUtils
	{

		static VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
		{
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			return commandBuffer;
		}

		static void EndSingleTimeCommands(VkDevice device, VkCommandPool commandPool,
			                              VkQueue queue, VkCommandBuffer commandBuffer)
		{

			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(queue);

			vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
		}

		static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice,
			                          uint32_t typeFilter, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

			uint32_t result = -1;

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if (typeFilter & (1 << i) &&
					(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					result = i;
				}
			}

			return result;
		}

		static void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
			                     VkDeviceSize size, VkBufferUsageFlags usage,
			                     VkMemoryPropertyFlags properties, VkBuffer& buffer,
			                     VkDeviceMemory& bufferMemory)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			{
				OP_ENGINE_ERROR("Failed to create the buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			{
				OP_ENGINE_ERROR("Failed to allocate buffer memory!");
			}

			vkBindBufferMemory(device, buffer, bufferMemory, 0);
		}

		static void CopyBuffer(VkDevice device, VkCommandPool commandPool,
			                   VkQueue queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
			VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);


			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			EndSingleTimeCommands(device, commandPool, queue, commandBuffer);

		}
	}
}