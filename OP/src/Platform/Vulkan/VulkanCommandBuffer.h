#pragma once

#include <vulkan/vulkan.hpp>

namespace OP
{

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer();
		VkCommandBuffer GetCommandBuffer();
		void RecordCommandBuffer();
	private:
		VkCommandBuffer m_CommandBuffer;
	};
}