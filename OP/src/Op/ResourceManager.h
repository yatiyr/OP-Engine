#pragma once

#include <string>
#include <Scene/Scene.h>
#include <filesystem>
#include <Renderer/Shader.h>
#include <Geometry/Model.h>

#include <Renderer/Material.h>
#include <Renderer/Texture.h>

#include <Renderer/EnvironmentMap.h>


#include <Platform/Vulkan/VulkanShaderModule.h>
#include <Platform/Vulkan/VulkanTexture.h>

namespace OP
{
	// Forward declaration
	class VulkanShaderModule;

	enum class ResourceTypes
	{
		R_MESH = 0,
		R_CUBEMAP = 1,
		R_PLAIN_TEXTURE = 2,
		R_ALBEDO_TEXTURE = 3,
		R_METALNESS_MAP = 4,
		R_ROUGHNESS_MAP = 5,
		R_NORMAL_MAP = 6,
		R_HEIGHT_MAP = 7,
		R_SHADER_PROGRAM = 8
	};

	class ResourceManager
	{
	public:
		static std::filesystem::path GetShaderCacheDirectory();

		static int Init(std::filesystem::path rootPath);

		static std::string GetNameFromID(uint32_t id);


		static int LoadIncludeShaders(std::filesystem::path shaderIncludeFilePath);
		static int LoadShaderSources(std::filesystem::path shaderIncludeFilePath);
		static int CompileShaders();

		static Ref<VulkanShaderModule> GetShader(std::string name);
		static Ref<VulkanTexture> GetTexture(std::string name);
		static int LoadTextures(std::filesystem::path texturePath);

		static std::string ResolveIncludes(const std::string& shaderSource, const std::string& fileName, std::unordered_map<std::string, bool>& includeMap, bool firstTime);


	private:
	};


}