#include <Precomp.h>
#include <Platform/Vulkan/VulkanRenderSystem.h>

#include <Platform/Vulkan/VulkanFramebuffer.h>
#include <Platform/Vulkan/VulkanGraphicsPipeline.h>
#include <Platform/Vulkan/VulkanRenderPass.h>
#include <Platform/Vulkan/VulkanCommandBuffer.h>
#include <Platform/Vulkan/VulkanBuffer.h>
#include <Platform/Vulkan/VulkanUniformBuffer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace OP
{

	// UniformBuffers
	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};


	struct Vert
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
	};

	struct VulkanRenderSystemData
	{
		Ref<VulkanGraphicsPipeline> Pipeline;
		std::vector<Ref<VulkanCommandBuffer>> CommandBuffers;
		Ref<VulkanVertexBuffer> VertexBuffer;
		Ref<VulkanIndexBuffer> IndexBuffer;
		std::vector<Ref<VulkanUniformBuffer>> UniformBuffers;
		Ref<VulkanDescriptorSetLayout> DescriptorSetLayout;
		uint32_t CurrentFrame = 0;


		// Turn into classes maybe
		VkDescriptorPool DescriptorPool;
		std::vector<VkDescriptorSet> DescriptorSets;
		Ref<VulkanTexture> Texture;

	} s_VulkanRenderData;


	void VulkanRenderSystem::CreateDescriptorPool()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		int maxFramesInFlight = VulkanContext::GetContext()->GetMaxFramesInFlight();

		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(maxFramesInFlight);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &s_VulkanRenderData.DescriptorPool) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to create descriptor tool!");
		}
	}

	void VulkanRenderSystem::CreateDescriptorSets()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		int maxFramesInFlight = VulkanContext::GetContext()->GetMaxFramesInFlight();

		std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, s_VulkanRenderData.DescriptorSetLayout->GetLayout());
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = s_VulkanRenderData.DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFramesInFlight);
		allocInfo.pSetLayouts = layouts.data();

		s_VulkanRenderData.DescriptorSets.resize(maxFramesInFlight);
		if (vkAllocateDescriptorSets(device, &allocInfo, s_VulkanRenderData.DescriptorSets.data()) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < maxFramesInFlight; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = s_VulkanRenderData.UniformBuffers[i]->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = s_VulkanRenderData.Texture->GetImageView();
			imageInfo.sampler = s_VulkanRenderData.Texture->GetSampler();

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = s_VulkanRenderData.DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = s_VulkanRenderData.DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

	}


	void VulkanRenderSystem::UpdateUniformBuffer(uint32_t currentImage, float ts)
	{
		VkExtent2D swapChainExtent = VulkanContext::GetContext()->GetSwapChainExtent();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), ts * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		s_VulkanRenderData.UniformBuffers[currentImage]->SetData(&ubo);
	}

	void VulkanRenderSystem::Init()
	{
		VulkanContext* context = VulkanContext::GetContext();

		s_VulkanRenderData.Texture = ResourceManager::GetTexture("texture");
		s_VulkanRenderData.Pipeline = std::make_shared<VulkanGraphicsPipeline>(ResourceManager::GetShader("sandbox"), context->GetSwapChainRenderPass());


		s_VulkanRenderData.Pipeline->ConfigureVertexInput(
			                                              {
														    { BufferElementType::OP_EL_FLOAT3, "a_Position", false },
														    { BufferElementType::OP_EL_FLOAT3, "a_Color", false},
															{ BufferElementType::OP_EL_FLOAT2, "a_TexCoord", false}
														  }, InputRate::VERTEX);

		s_VulkanRenderData.DescriptorSetLayout = std::make_shared<VulkanDescriptorSetLayout>();

		s_VulkanRenderData.Pipeline->InitializePipeline(s_VulkanRenderData.DescriptorSetLayout);


		const std::vector<Vert> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};

		const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		s_VulkanRenderData.VertexBuffer = std::make_shared<VulkanVertexBuffer>((void*)vertices.data(), vertices.size() * sizeof(Vert));
		s_VulkanRenderData.IndexBuffer = std::make_shared<VulkanIndexBuffer>((void*)indices.data(), indices.size() * sizeof(uint32_t), indices.size());
		CreateCommandBuffer();

		int maxFramesInFlight = context->GetMaxFramesInFlight();
		s_VulkanRenderData.UniformBuffers.resize(maxFramesInFlight);

		for (int i = 0; i < maxFramesInFlight; i++)
		{
			s_VulkanRenderData.UniformBuffers[i] = std::make_shared<VulkanUniformBuffer>(sizeof(UniformBufferObject));
		}

		CreateDescriptorPool();
		CreateDescriptorSets();
	}

	void VulkanRenderSystem::Cleanup()
	{		
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		vkDeviceWaitIdle(device);
		//s_VulkanRenderData.SwapchainFramebuffers.clear();

		vkDestroyDescriptorPool(device, s_VulkanRenderData.DescriptorPool, nullptr);
	}

	void VulkanRenderSystem::Render(float ts)
	{
		VulkanContext* context = VulkanContext::GetContext();

		VkDevice device = context->GetDevice();
		std::vector<VkFence> inFlightFences = context->GetInFlightFences();
		std::vector<VkSemaphore> imageAvailableSemaphores = context->GetImageAvailableSemaphores();
		std::vector<VkSemaphore> renderFinishedSemaphores = context->GetRenderFinishedSemaphores();
		VkSwapchainKHR swapchain = context->GetSwapchain();
		VkExtent2D extent = context->GetSwapChainExtent();
		VkQueue graphicsQueue = context->GetGraphicsQueue();
		VkQueue presentQueue = context->GetPresentQueue();

		vkWaitForFences(device, 1, &inFlightFences[s_VulkanRenderData.CurrentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(device, 1, &inFlightFences[s_VulkanRenderData.CurrentFrame]);

		uint32_t imageIndex; 
		vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[s_VulkanRenderData.CurrentFrame], VK_NULL_HANDLE, &imageIndex);

		s_VulkanRenderData.CommandBuffers[s_VulkanRenderData.CurrentFrame]->ResetCommandBuffer();
		s_VulkanRenderData.CommandBuffers[s_VulkanRenderData.CurrentFrame]->RecordCommandBuffer(context->GetSwapChainRenderPass(),
										context->GetSwapChainFramebuffers()[imageIndex],
										s_VulkanRenderData.Pipeline,
			                            s_VulkanRenderData.VertexBuffer,
										s_VulkanRenderData.IndexBuffer,
										s_VulkanRenderData.DescriptorSets[s_VulkanRenderData.CurrentFrame],
										extent);

		UpdateUniformBuffer(s_VulkanRenderData.CurrentFrame, ts);

		// TODO: CLEAN THIS UP
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[s_VulkanRenderData.CurrentFrame]};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &s_VulkanRenderData.CommandBuffers[s_VulkanRenderData.CurrentFrame]->GetCommandBuffer();

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[s_VulkanRenderData.CurrentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;



		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[s_VulkanRenderData.CurrentFrame]) != VK_SUCCESS)
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

		s_VulkanRenderData.CurrentFrame = (s_VulkanRenderData.CurrentFrame + 1) % context->GetMaxFramesInFlight();
	}

	void VulkanRenderSystem::CreateCommandBuffer()
	{
		VulkanContext* context = VulkanContext::GetContext();
		int maxFramesInFlight = context->GetMaxFramesInFlight();

		for (uint32_t i = 0; i < maxFramesInFlight; i++)
		{
			s_VulkanRenderData.CommandBuffers.push_back(std::make_shared<VulkanCommandBuffer>());
		}
	}

	void VulkanRenderSystem::RecreateSwapchain()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();
		vkDeviceWaitIdle(device);

		VulkanContext::GetContext()->CleanupSwapchain();

		VulkanContext::GetContext()->CreateSwapchain();
		VulkanContext::GetContext()->CreateImageViews();
		VulkanContext::GetContext()->CreateSwapchainFramebuffers();

	}

}