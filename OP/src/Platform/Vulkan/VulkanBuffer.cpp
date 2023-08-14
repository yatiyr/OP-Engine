#include <Precomp.h>
#include <Platform/Vulkan/VulkanBuffer.h>
#include <Platform/Vulkan/VulkanContext.h>

#include <Platform/Vulkan/VulkanBufferUtils.h>

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


		BufferUtils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMemory);

		void* d;
		vkMapMemory(device, m_StagingBufferMemory, 0, size, 0, &d);
			memcpy(d, data, (size_t)size);
		vkUnmapMemory(device, m_StagingBufferMemory);


		BufferUtils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VertexBuffer, m_VertexBufferMemory);


		BufferUtils::CopyBuffer(m_StagingBuffer, m_VertexBuffer, size);

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

		BufferUtils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_StagingBuffer, m_StagingBufferMemory);

		void* d;
		vkMapMemory(device, m_StagingBufferMemory, 0, size, 0, &d);
		memcpy(d, data, (size_t)size);
		vkUnmapMemory(device, m_StagingBufferMemory);

		BufferUtils::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

		BufferUtils::CopyBuffer(m_StagingBuffer, m_IndexBuffer, size);

		vkDestroyBuffer(device, m_StagingBuffer, nullptr);
		vkFreeMemory(device, m_StagingBufferMemory, nullptr);
	}


}