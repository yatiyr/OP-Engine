#pragma once


#include <Platform/Vulkan/VulkanShaderModule.h>


namespace OP
{

	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(const std::map<uint32_t, VkShaderModule>& vulkanShaderModules);
		~VulkanGraphicsPipeline();

	private:
		VkPipelineLayout m_PipelineLayout;
	};
}