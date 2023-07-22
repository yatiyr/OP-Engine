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