#pragma once

#include <Platform/Vulkan/VulkanContext.h>
#include <map>

namespace OP
{
	class VulkanShaderModule
	{
	public:
		VulkanShaderModule(const std::string& name,const std::map<uint32_t, std::string>& shaderTypeMap);
		~VulkanShaderModule();
	private:
		void CompileOrGetBinaries(const std::map<uint32_t, std::string>& shaderTypeMap);

		void Reflect(uint32_t stage, const std::vector<uint32_t>& shaderData);
		VkShaderModule CreateShaderModule(uint32_t stage, const std::vector<uint32_t>& code);

		std::string ReadFile(const std::string& filePath);
		std::string m_Name;

		std::map<uint32_t, std::vector<uint32_t>> m_VulkanSPIRV;

		std::map<uint32_t, VkShaderModule> m_ShaderModules;
	};
}