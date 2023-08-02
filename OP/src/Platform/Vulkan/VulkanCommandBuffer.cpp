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

	VkCommandBuffer& VulkanCommandBuffer::GetCommandBuffer()
	{
		return m_CommandBuffer;
	}

	void VulkanCommandBuffer::ResetCommandBuffer()
	{
		vkResetCommandBuffer(m_CommandBuffer, 0);
	}

	void VulkanCommandBuffer::RecordCommandBuffer(Ref<VulkanRenderPass> renderpass,
		                                          Ref<VulkanFramebuffer> framebuffer,
		                                          Ref<VulkanGraphicsPipeline> pipeline,
		                                          Ref<VulkanVertexBuffer> vertexBuffer,
												  Ref<VulkanIndexBuffer> indexBuffer,
		                                          VkExtent2D extent,
												  uint32_t indexSize)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to begin recording the command buffer!");
		}

		// Start renderpass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderpass->GetVkRenderPass();
		renderPassInfo.framebuffer = framebuffer->GetVkFramebuffer();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;

		// ClearColor
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		// Begin the render pass
		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

		// Fixed functions
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(extent.width);
		viewport.height = static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

		// TODO: WILL BE CHANGED
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &vertexBuffer->GetBuffer(), offsets);
		vkCmdBindIndexBuffer(m_CommandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(m_CommandBuffer, indexSize, 1, 0, 0, 0);

		vkCmdEndRenderPass(m_CommandBuffer);

		if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to record command buffer!");
		}
	}

}