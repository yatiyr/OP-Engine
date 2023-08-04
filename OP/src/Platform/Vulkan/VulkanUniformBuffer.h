#pragma once

#include <vulkan/vulkan.hpp>
#include <Platform/Vulkan/VulkanBuffer.h>

namespace OP
{
	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout();
		~VulkanDescriptorSetLayout();
		VkDescriptorSetLayout& GetLayout();
	private:
		VkDescriptorSetLayout m_DescriptorSetLayout;
	};


	class VulkanUniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size);
		~VulkanUniformBuffer();
		void SetData(void* data);
		VkBuffer& GetBuffer();
	private:
		VkBuffer m_UniformBuffer;
		VkDeviceMemory m_UniformBufferMemory;
		void* m_UniformBufferMapped;
		size_t size;
	};
}