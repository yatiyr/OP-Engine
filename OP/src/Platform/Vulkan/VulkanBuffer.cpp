#include <Precomp.h>
#include <Platform/Vulkan/VulkanBuffer.h>
#include <Platform/Vulkan/VulkanContext.h>

namespace OP
{
	// Utility functions
	namespace Utils
	{
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDevice physicalDevice = VulkanContext::GetContext()->GetPhysicalDevice();
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

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
			VkDevice device = VulkanContext::GetContext()->GetDevice();

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
			allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			{
				OP_ENGINE_ERROR("Failed to allocate buffer memory!");
			}

			vkBindBufferMemory(device, buffer, bufferMemory, 0);
		}

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
			VkCommandPool commandPool = VulkanContext::GetContext()->GetCommandPool();
			VkDevice device = VulkanContext::GetContext()->GetDevice();
			VkQueue queue = VulkanContext::GetContext()->GetGraphicsQueue();

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

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(queue);

			vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);


		}
	}



	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size)
	{
		Populate(data, size);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		vkDestroyBuffer(device, m_VertexBuffer, nullptr);
		vkFreeMemory(device, m_VertexBufferMemory, nullptr);
	}

	void VulkanVertexBuffer::SetData(void* data, uint32_t size)
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		if (m_VertexBuffer != VK_NULL_HANDLE)
			vkDestroyBuffer(device, m_VertexBuffer, nullptr);
		if (m_VertexBufferMemory != VK_NULL_HANDLE)
			vkFreeMemory(device, m_VertexBufferMemory, nullptr);

		Populate(data, size);
			
	}

	VkBuffer& VulkanVertexBuffer::GetBuffer()
	{
		return m_VertexBuffer;
	}

	void VulkanVertexBuffer::Populate(void* data, uint32_t size)
	{

		VkDevice device = VulkanContext::GetContext()->GetDevice();


		Utils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMemory);

		void* d;
		vkMapMemory(device, m_StagingBufferMemory, 0, size, 0, &d);
			memcpy(d, data, (size_t)size);
		vkUnmapMemory(device, m_StagingBufferMemory);


		Utils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VertexBuffer, m_VertexBufferMemory);


		Utils::CopyBuffer(m_StagingBuffer, m_VertexBuffer, size);

		vkDestroyBuffer(device, m_StagingBuffer, nullptr);
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint32_t size)
	{
		Populate(data, size);
	}


	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		vkDestroyBuffer(device, m_IndexBuffer, nullptr);
		vkFreeMemory(device, m_IndexBufferMemory, nullptr);
	}

	void VulkanIndexBuffer::SetData(void* data, uint32_t size)
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		if (m_IndexBuffer != VK_NULL_HANDLE)
			vkDestroyBuffer(device, m_IndexBuffer, nullptr);
		if (m_IndexBufferMemory != VK_NULL_HANDLE)
			vkFreeMemory(device, m_IndexBufferMemory, nullptr);

		Populate(data, size);
	}

	VkBuffer& VulkanIndexBuffer::GetBuffer()
	{
		return m_IndexBuffer;
	}

	void VulkanIndexBuffer::Populate(void* data, uint32_t size)
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();

		Utils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMemory);

		void* d;
		vkMapMemory(device, m_StagingBufferMemory, 0, size, 0, &d);
		memcpy(d, data, (size_t)size);
		vkUnmapMemory(device, m_StagingBufferMemory);

		Utils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

		Utils::CopyBuffer(m_StagingBuffer, m_IndexBuffer, size);

		vkDestroyBuffer(device, m_StagingBuffer, nullptr);
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);
	}


}