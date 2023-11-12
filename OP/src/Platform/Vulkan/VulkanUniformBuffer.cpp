#include <Precomp.h>
#include <Platform/Vulkan/VulkanUniformBuffer.h>
#include <Platform/Vulkan/VulkanContext.h>


namespace OP
{

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
	}

	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to create descriptor set layout!");
		}
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
	}

	VkDescriptorSetLayout& VulkanDescriptorSetLayout::GetLayout()
	{
		return m_DescriptorSetLayout;
	}

	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size)
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();

		VkDeviceSize s = size;

		Utils::CreateBuffer(s, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffer, m_UniformBufferMemory);

		vkMapMemory(device, m_UniformBufferMemory, 0, s, 0, &m_UniformBufferMapped);

		this->size = size;
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		vkDestroyBuffer(device, m_UniformBuffer, nullptr);
		vkFreeMemory(device, m_UniformBufferMemory, nullptr);
	}

	void VulkanUniformBuffer::SetData(void* data)
	{
		memcpy(m_UniformBufferMapped, data, size);
	}

	VkBuffer& VulkanUniformBuffer::GetBuffer()
	{
		return m_UniformBuffer;
	}


}