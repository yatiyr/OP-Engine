#pragma once

#include <vulkan/vulkan.hpp>

namespace OP
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass();
		~VulkanRenderPass();

		VkRenderPass GetVkRenderPass();
	private:
		VkRenderPass m_RenderPass;
	};
}