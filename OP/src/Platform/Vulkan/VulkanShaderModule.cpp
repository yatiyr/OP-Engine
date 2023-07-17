#include <Precomp.h>

#include <shaderc/shaderc.hpp>
#include <SpirV_Cross/spirv_cross.hpp>
#include <SpirV_Cross/spirv_glsl.hpp>
#include <Platform/Vulkan/VulkanShaderModule.h>

#include <vulkan/vulkan.hpp>

#include <Op/ResourceManager.h>

namespace OP
{

	enum class VK_SHADER_TYPE
	{
		VK_NONE,
		VK_VERTEX_SHADER,
		VK_FRAGMENT_SHADER,
		VK_GEOMETRY_SHADER
	};


	namespace Utils
	{
		static VK_SHADER_TYPE ShaderTypeFromStr(const std::string& type)
		{
			if (type == "vertex")
				return VK_SHADER_TYPE::VK_GEOMETRY_SHADER;
			if (type == "fragment" || type == "pixel")
				return VK_SHADER_TYPE::VK_FRAGMENT_SHADER;
			if (type == "geometry")
				return VK_SHADER_TYPE::VK_GEOMETRY_SHADER;

			return VK_SHADER_TYPE::VK_NONE;
		}

		static shaderc_shader_kind VKShaderStageToShaderC(uint32_t stage)
		{
			switch (stage)
			{
			    case 0: return shaderc_glsl_vertex_shader;
				case 1: return shaderc_glsl_fragment_shader;
				case 2: return shaderc_glsl_geometry_shader;
			}

			return (shaderc_shader_kind)0;
		}

		static const char* VKShaderStageToString(uint32_t stage)
		{
			switch (stage)
			{
			case 0: return "VK_VERTEX_SHADER";
			case 1: return "VK_FRAGMENT_SHADER";
			case 2: return "VK_GEOMETRY_SHADER";
			}

			return "";
		}

		static const char* GetCacheDirectory()
		{
			return "assets/cache/shader/vulkan";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* VKShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case 0: return ".cached_vulkan.vert";
				case 1: return ".cached_vulkan.frag";
				case 2: return ".cached_vulkan.geom";
			}

			OP_ENGINE_ASSERT(false);
			return "";
		}
	}

	VulkanShaderModule::VulkanShaderModule(const std::string& name, const std::map<uint32_t, std::string>& shaderTypeMap) : m_Name(name)
	{
		CompileOrGetBinaries(shaderTypeMap);

		for (auto&& [stage, code] : m_VulkanSPIRV)		
			CreateShaderModule(stage, code);		
	}

	VulkanShaderModule::~VulkanShaderModule()
	{
		VkDevice device = VulkanContext::GetContext()->GetDevice();

		for (auto&& [stage, module] : m_ShaderModules)
		{
			vkDestroyShaderModule(device, module, nullptr);
		}
	}

	void VulkanShaderModule::CompileOrGetBinaries(const std::map<uint32_t, std::string>& shaderTypeMap)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = ResourceManager::GetShaderCacheDirectory() / "vulkan";

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();

		for (auto&& [stage, source] : shaderTypeMap)
		{
			std::filesystem::path cachedPath = cacheDirectory / (m_Name + Utils::VKShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::VKShaderStageToShaderC(stage), m_Name.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					OP_ENGINE_ERROR(module.GetErrorMessage()); 
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void VulkanShaderModule::Reflect(uint32_t stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		OP_ENGINE_TRACE("VK Shader Reflect - {0} {1}", Utils::VKShaderStageToString(stage), m_Name);
		OP_ENGINE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		OP_ENGINE_TRACE("    {0} resources", resources.sampled_images.size());

		OP_ENGINE_TRACE("Unform Buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			OP_ENGINE_TRACE("  {0}", resource.name);
			OP_ENGINE_TRACE("    Size = {0}", bufferSize);
			OP_ENGINE_TRACE("    Binding = {0}", binding);
			OP_ENGINE_TRACE("    Members = {0}", memberCount);
		}
	}

	VkShaderModule VulkanShaderModule::CreateShaderModule(uint32_t stage, const std::vector<uint32_t>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size() * sizeof(uint32_t);
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(VulkanContext::GetContext()->GetDevice(), &createInfo, nullptr, &m_ShaderModules[stage]) != VK_SUCCESS)
		{
			OP_ENGINE_ERROR("Failed to create shader module!");
		}

		return m_ShaderModules[stage];
	}

	std::string VulkanShaderModule::ReadFile(const std::string& filePath)
	{
		std::string result;

		std::ifstream in(filePath, std::ios::in | std::ios::binary);

		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				OP_ENGINE_ERROR("Could not read from file '{0}'", filePath);
			}
		}
		else
		{
			OP_ENGINE_ERROR("Could not open file '{0}'", filePath);
		}

		return result;
	}

}