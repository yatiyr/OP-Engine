#include <Precomp.h>
#include <Platform/Vulkan/VulkanRenderSystem.h>

#include <Platform/Vulkan/VulkanFramebuffer.h>
#include <Platform/Vulkan/VulkanGraphicsPipeline.h>
#include <Platform/Vulkan/VulkanRenderPass.h>
#include <Platform/Vulkan/VulkanCommandBuffer.h>

namespace OP
{
	struct VulkanRenderSystemData
	{
		Ref<VulkanGraphicsPipeline> Pipeline;
		Ref<VulkanRenderPass> RenderPass;
		std::vector<Ref<VulkanFramebuffer>> SwapchainFramebuffers;
		Ref<VulkanCommandBuffer> CommandBuffer;
	} s_VulkanRenderData;



	void VulkanRenderSystem::Init()
	{
		s_VulkanRenderData.RenderPass = std::make_shared<VulkanRenderPass>();
		s_VulkanRenderData.Pipeline = std::make_shared<VulkanGraphicsPipeline>(ResourceManager::GetShader("sandbox"), s_VulkanRenderData.RenderPass);

		CreateFramebuffers();
		CreateCommandBuffer();
	}

	void VulkanRenderSystem::Cleanup()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		for (auto framebuffer : s_VulkanRenderData.SwapchainFramebuffers)
		{
			framebuffer->~VulkanFramebuffer();
		}
	}

	void VulkanRenderSystem::Render()
	{
		VulkanContext* context = VulkanContext::GetContext();

		VkDevice device = context->GetDevice();
		VkFence inFlightFence = context->GetInFlightFence();
		VkSemaphore imageAvailableSemaphore = context->GetImageAvailableSemaphore();
		VkSemaphore renderFinishedSemaphore = context->GetRenderFinishedSemaphore();
		VkSwapchainKHR swapchain = context->GetSwapchain();
		VkExtent2D extent = context->GetSwapChainExtent();
		VkQueue graphicsQueue = context->GetGraphicsQueue();
		VkQueue presentQueue = context->GetPresentQueue();

		vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		s_VulkanRenderData.CommandBuffer->ResetCommandBuffer();
		s_VulkanRenderData.CommandBuffer->RecordCommandBuffer(s_VulkanRenderData.RenderPass,
										s_VulkanRenderData.SwapchainFramebuffers[imageIndex],
										s_VulkanRenderData.Pipeline,
										extent);

		// TODO: CLEAN THIS UP
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &s_VulkanRenderData.CommandBuffer->GetCommandBuffer();

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;


		vkResetFences(device, 1, &inFlightFence);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.pResults = nullptr;

		VkSwapchainKHR swapchains[] = { swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(presentQueue, &presentInfo);

	}

	// Try to move framebuffer to a separate class
	void VulkanRenderSystem::CreateFramebuffers()
	{
		VulkanContext* context = VulkanContext::GetContext();

		std::vector<VkImageView> swapchainImageViews = context->GetSwapChainImageViews();
		VkExtent2D swapchainExtent = context->GetSwapChainExtent();
		VkDevice device = context->GetDevice();

		s_VulkanRenderData.SwapchainFramebuffers.resize(swapchainImageViews.size());

		for (uint32_t i = 0; i < swapchainImageViews.size(); i++)
		{
			VkImageView attachments[] =
			{
				swapchainImageViews[i]
			};

			s_VulkanRenderData.SwapchainFramebuffers[i] = std::make_shared<VulkanFramebuffer>(s_VulkanRenderData.RenderPass,
				                                                                              attachments,
				                                                                              swapchainExtent.width, swapchainExtent.height);

		}

	}

	void VulkanRenderSystem::CreateCommandBuffer()
	{
		s_VulkanRenderData.CommandBuffer = std::make_shared<VulkanCommandBuffer>();
	}

}