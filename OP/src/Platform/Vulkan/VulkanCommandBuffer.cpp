#include <Precomp.h>

#include <Platform/Vulkan/VulkanContext.h>
#include <Platform/Vulkan/VulkanCommandBuffer.h>

namespace OP
{
	VulkanCommandBuffer::VulkanCommandBuffer()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		VkCommandPool commandPool = VulkanContext::GetContext()->GetCommandPool();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to allocate command buffers!");
		}
	}

	VkCommandBuffer VulkanCommandBuffer::GetCommandBuffer()
	{
		return m_CommandBuffer;
	}

}