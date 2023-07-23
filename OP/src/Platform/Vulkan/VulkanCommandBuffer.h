#pragma once

#include <vulkan/vulkan.hpp>

#include <Platform/Vulkan/VulkanRenderPass.h>
#include <Platform/Vulkan/VulkanFramebuffer.h>

namespace OP
{

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer();
		VkCommandBuffer& GetCommandBuffer();
		void ResetCommandBuffer();
		void RecordCommandBuffer(Ref<VulkanRenderPass> renderpass, Ref<VulkanFramebuffer> framebuffer, Ref<VulkanGraphicsPipeline> pipeline, VkExtent2D extent);
	private:
		VkCommandBuffer m_CommandBuffer;
	};
}