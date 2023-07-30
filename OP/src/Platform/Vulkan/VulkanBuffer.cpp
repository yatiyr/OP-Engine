#include <Precomp.h>
#include <Platform/Vulkan/VulkanBuffer.h>
#include <Platform/Vulkan/VulkanContext.h>

namespace OP
{
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

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkDevice device = VulkanContext::GetContext()->GetDevice();

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Could not create the vertex buffer");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, m_VertexBuffer, &memRequirements);

		// Memory Alloc
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(device, m_VertexBuffer, m_VertexBufferMemory, 0);
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

	uint32_t VulkanVertexBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

	void VulkanVertexBuffer::Populate(void* data, uint32_t size)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkDevice device = VulkanContext::GetContext()->GetDevice();

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Could not create the vertex buffer");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, m_VertexBuffer, &memRequirements);

		// Memory Alloc
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(device, m_VertexBuffer, m_VertexBufferMemory, 0);

		void* mem;
		vkMapMemory(device, m_VertexBufferMemory, 0, bufferInfo.size, 0, &mem);
		memcpy(mem, data, (size_t)bufferInfo.size);
		vkUnmapMemory(device, m_VertexBufferMemory);
	}

}