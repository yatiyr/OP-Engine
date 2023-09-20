#pragma once

#include <Platform/Vulkan/VulkanRenderPass.h>
#include <Platform/Vulkan/VulkanShaderModule.h>
#include <Platform/Vulkan/VulkanBuffer.h>
#include <Platform/Vulkan/VulkanUniformBuffer.h>

namespace OP
{

	enum class InputRate : uint8_t
	{
		VERTEX = 0,
		INSTANCE = 1
	};

	static VkVertexInputRate OpToVkInputRate(InputRate inputRate)
	{
		switch (inputRate)
		{
			case InputRate::VERTEX:
				return VK_VERTEX_INPUT_RATE_VERTEX;
			case InputRate::INSTANCE:
				return VK_VERTEX_INPUT_RATE_INSTANCE;
			default:
				return VK_VERTEX_INPUT_RATE_MAX_ENUM;
		}
	}

	struct BindingAttributeDescs
	{
		std::vector<VkVertexInputBindingDescription> Bindings;
		std::vector<VkVertexInputAttributeDescription> Attributes;
	};

	// Forward declaration
	class VulkanShaderModule;

	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(Ref<VulkanShaderModule> shaders, Ref<VulkanRenderPass> renderPass);
		~VulkanGraphicsPipeline();


		void InitializePipeline(Ref<VulkanDescriptorSetLayout> descriptorSetLayout, AttachmentSample samples);
		void ConfigureVertexInput(const VertexInput& input, InputRate inputRate);

		VkPipeline GetPipeline();
		VkPipelineLayout GetPipelineLayout();
	private:
		BindingAttributeDescs m_VertexInputDescs;
		Ref<VulkanShaderModule> m_Shaders;
		Ref<VulkanRenderPass> m_RenderPass;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	};
}