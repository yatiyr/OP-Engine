#pragma once

#include <vulkan/vulkan.hpp>

#include <Platform/Vulkan/VulkanRenderPass.h>
#include <Platform/Vulkan/VulkanFramebuffer.h>
#include <Platform/Vulkan/VulkanBuffer.h>

namespace OP
{

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer();
		VkCommandBuffer& GetCommandBuffer();
		void ResetCommandBuffer();
		void RecordCommandBuffer(Ref<VulkanRenderPass> renderpass,
								 Ref<VulkanFramebuffer> framebuffer,
								 Ref<VulkanGraphicsPipeline> pipeline,
								 Ref<VulkanVertexBuffer> vertexBuffer,
								 Ref<VulkanIndexBuffer> indexBuffer,
								 VkExtent2D extent,
			                     uint32_t indexSize);
	private:
		VkCommandBuffer m_CommandBuffer;
	};
}