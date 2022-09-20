#include <string>
#include <Scene/Scene.h>
#include <filesystem>
#include <Renderer/Shader.h>
#include <Geometry/Model.h>

#include <Renderer/Material.h>
#include <Renderer/Texture.h>

#include <Renderer/EnvironmentMap.h>

namespace OP
{

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

		static Ref<Shader> GetShader(std::string name);
		static Ref<Model> GetModel(std::string name);
		static Ref<Texture> GetTexture(std::string name);
		static Ref<EnvironmentMap> GetEnvironmentMap(std::string name);
		static Ref<Material> GetMaterial(std::string name);

		static int LoadModels(std::filesystem::path meshFilePath);
		static int RegisterModelResources();

		static std::string ResolveIncludes(const std::string& shaderSource, const std::string& fileName, std::unordered_map<std::string, bool>& includeMap, bool firstTime);
		// Mesh Related functions
		static int AddMesh();
		static Ref<Mesh> GetMesh(std::string name);
		static const std::unordered_map<uint32_t, Ref<Mesh>>& GetMeshMap();
		static const std::unordered_map<uint32_t, Ref<EnvironmentMap>>& GetEnvironmentMaps();

		static int LoadMaterials(std::filesystem::path materialsFilePath);

		// Texture related functions
		static int LoadEnvironmentMaps(std::filesystem::path texturesFilePath);
		static int LoadTextures(std::filesystem::path texturesFilePath);

	private:
	};


}