#pragma once

#include <Platform/Vulkan/VulkanRenderPass.h>
#include <Platform/Vulkan/VulkanShaderModule.h>


namespace OP
{
	// Forward declaration
	class VulkanShaderModule;

	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(Ref<VulkanShaderModule> shaders, Ref<VulkanRenderPass> renderPass);
		~VulkanGraphicsPipeline();

	private:
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	};
}