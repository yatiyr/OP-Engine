#include <Precomp.h>
#include <Opium/ResourceManager.h>

#include <Geometry/Mesh.h>
#include <Geometry/Icosphere.h>
#include <Geometry/Plane.h>
#include <Geometry/RadialSphere.h>
#include <Geometry/Cube.h>
#include <Geometry/Model.h>


#include <Renderer/Texture.h>

#include <Renderer/Shader.h>
#include <Renderer/Material.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace OP
{


	struct ResourceManagerData
	{
		std::unordered_map<uint32_t, Ref<Mesh>> Meshes;
		std::unordered_map<uint32_t, Ref<Model>> Models;
		std::unordered_map<uint32_t, Ref<Material>> Materials;

		std::unordered_map<uint32_t, Ref<Texture>> CubeMaps;
		std::unordered_map<uint32_t, Ref<Texture>> Textures;

		std::unordered_map<uint32_t, Ref<Shader>> Shaders;
		std::unordered_map<std::string, std::string> ShaderSources;
		std::unordered_map<std::string, std::string> IncludeShaderSources;


		std::unordered_map<std::string, uint32_t> StringLookupTable;
		std::unordered_map<uint32_t, std::string> IDLookupTable;

		uint32_t counter = 0;

		std::filesystem::path root;
	} s_ResourceManagerData;


	static uint32_t Allocate(std::string name)
	{
		s_ResourceManagerData.StringLookupTable[name] = s_ResourceManagerData.counter;
		s_ResourceManagerData.IDLookupTable[s_ResourceManagerData.counter] = name;
		s_ResourceManagerData.counter++;

		return s_ResourceManagerData.counter - 1;
	}
	
	int ResourceManager::CompileShaders()
	{

		OP_ENGINE_WARN("\tCompiling Shaders");

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		// Iterate over shader sources
		for (auto [key, src] : s_ResourceManagerData.ShaderSources)
		{

			std::vector <std::string> sources;
			
			size_t tokenPos = src.find(typeToken, 0);

			while (tokenPos != std::string::npos)
			{
				size_t eol = src.find_first_of("\r\n", tokenPos);
				size_t begin = tokenPos + typeTokenLength + 1;
				std::string type = src.substr(begin, eol - begin);

				size_t nextLineTokenPos = src.find_first_not_of("\r\n", eol);
				tokenPos = src.find(typeToken, nextLineTokenPos);

				std::string source = src.substr(nextLineTokenPos, tokenPos - (nextLineTokenPos == std::string::npos ? src.size() - 1 : nextLineTokenPos));

				std::unordered_map<std::string, bool> includeMap;

				std::string resolvedSource = ResourceManager::ResolveIncludes(source, "", includeMap, true);
				sources.push_back(resolvedSource);
			}

			switch (sources.size())
			{
				// Compute ?
				case 1:
					break;

				// Vertex Fragment
				case 2:
					s_ResourceManagerData.StringLookupTable[key] = s_ResourceManagerData.counter;
					s_ResourceManagerData.IDLookupTable[s_ResourceManagerData.counter] = key;
					s_ResourceManagerData.Shaders[s_ResourceManagerData.counter] = Shader::Create(key, sources[0], sources[1]);
					break;
				// Vertex Fragment Geometry
				case 3:
					s_ResourceManagerData.StringLookupTable[key] = s_ResourceManagerData.counter;
					s_ResourceManagerData.IDLookupTable[s_ResourceManagerData.counter] = key;
					s_ResourceManagerData.Shaders[s_ResourceManagerData.counter] = Shader::Create(key, sources[0], sources[1], sources[2]);
					break;
			}

			s_ResourceManagerData.counter++;

		}

		OP_ENGINE_WARN("\tShaders have been compiled!");
		return 0;
	}

	Ref<Model> ResourceManager::GetModel(std::string name)
	{
		uint32_t modelHandle = s_ResourceManagerData.StringLookupTable[name];
		return s_ResourceManagerData.Models[modelHandle];
	}

	Ref<Texture> ResourceManager::GetTexture(std::string name)
	{
		uint32_t modelHandle = s_ResourceManagerData.StringLookupTable[name];
		return s_ResourceManagerData.Textures[modelHandle];
	}

	Ref<Material> ResourceManager::GetMaterial(std::string name)
	{
		uint32_t modelHandle = s_ResourceManagerData.StringLookupTable[name];
		return s_ResourceManagerData.Materials[modelHandle];
	}

	Ref<Shader> ResourceManager::GetShader(std::string name)
	{
		uint32_t shaderHandle =  s_ResourceManagerData.StringLookupTable[name];
		return s_ResourceManagerData.Shaders[shaderHandle];
	}
	 
	int ResourceManager::LoadModels(std::filesystem::path meshFilePath)
	{
		uint32_t count = 0;
		OP_ENGINE_WARN("\tLoading Models");

		try
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(meshFilePath))
			{
				if (entry.is_regular_file() && (
					entry.path().extension() == ".gltf") || 
					entry.path().extension() == ".fbx" || 
					entry.path().extension() == ".dae")
				{
					// Read the file and put it into unordered map with key
					// as its filename
					std::filesystem::path entryPath = entry.path();
					std::ifstream modelFile(entryPath);
					std::stringstream buffer;
					buffer << modelFile.rdbuf();

					OP_ENGINE_INFO("\t\tFileName {0}", entryPath.filename());

					Assimp::Importer import;
					const aiScene* scene = import.ReadFile(entryPath.string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_DropNormals);

					if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
					{
						OP_ENGINE_ERROR("ERROR::ASSIMP::{0}", import.GetErrorString());
						return true;
					}

					Ref<Model> model = Model::Create(scene->mRootNode, scene);

					s_ResourceManagerData.StringLookupTable[entryPath.stem().string()] = s_ResourceManagerData.counter;
					s_ResourceManagerData.Models[s_ResourceManagerData.counter] = model;

					s_ResourceManagerData.counter++;

					modelFile.close();

					count++;
				}
			}

			OP_ENGINE_INFO("\t\tTotal number of processed Models : {0}", count);
		}
		catch (const std::exception&)
		{
			OP_ENGINE_ERROR("\tFailed to load Models.")
				return 1;
		}

		OP_ENGINE_WARN("\tModels have been loaded")
			return 0;
	}

	int ResourceManager::RegisterModelResources()
	{
		return 0;
	}

	std::string ResourceManager::ResolveIncludes(const std::string& shaderSource, const std::string& fileName, std::unordered_map<std::string, bool>& includeMap , bool firstTime)
	{
		std::string includeToken = "#include ";
		std::stringstream ssSource;
		std::string line = "";
		std::string resolvedSource = "";

		if (firstTime)
			ssSource = std::stringstream(shaderSource);
		else
			ssSource = std::stringstream(s_ResourceManagerData.IncludeShaderSources[fileName]);


		while (std::getline(ssSource, line))
		{
			if (line.find(includeToken) != line.npos)
			{
				line.erase(0, includeToken.size());

				// Get include contents from resource manager
				if (!includeMap[line])
				{
					includeMap[line] = true;
					resolvedSource += ResourceManager::ResolveIncludes(shaderSource, line, includeMap, false);
				}
					

				continue;
			}

			resolvedSource += line + "\n";
		}


		return resolvedSource;
	}

	int ResourceManager::LoadShaderSources(std::filesystem::path shaderSourcePath)
	{
		uint32_t count = 0;
		OP_ENGINE_WARN("\tLoading Shader Sources");
		try
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(shaderSourcePath))
			{
				if (entry.is_regular_file())
				{
					// Read the file and put it into unordered map with key
					// as its filename
					std::filesystem::path entryPath = entry.path();
					std::ifstream shaderIncludeFile(entryPath);
					std::stringstream buffer;
					buffer << shaderIncludeFile.rdbuf();

					s_ResourceManagerData.ShaderSources[entryPath.filename().string()] = buffer.str();

					OP_ENGINE_INFO("\t\tFileName {0}", entryPath.filename());

					shaderIncludeFile.close();

					count++;
				}
			}

			OP_ENGINE_INFO("\tTotal number of shader source files : {0}", count);
		}
		catch (const std::exception&)
		{
			OP_ENGINE_ERROR("\tFailed to load shader source files.")
			return 1;
		}
		
		OP_ENGINE_WARN("\tShader sources have been loaded!");
		return 0;
	}
	
	int ResourceManager::LoadIncludeShaders(std::filesystem::path shaderIncludeFilePath)
	{
		uint32_t count = 0;
		OP_ENGINE_WARN("\tLoading Include Shaders");

		try
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(shaderIncludeFilePath))
			{
				if (entry.is_regular_file())
				{
					// Read the file and put it into unordered map with key
					// as its filename
					std::filesystem::path entryPath = entry.path();
					std::ifstream shaderIncludeFile(entryPath);
					std::stringstream buffer;
					buffer << shaderIncludeFile.rdbuf();

					s_ResourceManagerData.IncludeShaderSources[entryPath.filename().string()] = buffer.str();

					OP_ENGINE_INFO("\t\tFileName {0}", entryPath.filename());

					shaderIncludeFile.close();

					count++;
				}
			}

			OP_ENGINE_INFO("\t\tTotal number of include shader files : {0}", count);
		}
		catch (const std::exception&)
		{
			OP_ENGINE_ERROR("\tFailed to load include shaders.")
			return 1;
		}

		OP_ENGINE_WARN("\tInclude shaders have been loaded")
		return 0;
	}

	std::filesystem::path ResourceManager::GetShaderCacheDirectory()
	{
		return s_ResourceManagerData.root / "assets" / "cache" / "shader";
	}

	// Reads the structure in root file path and loads the resources
	int ResourceManager::Init(std::filesystem::path rootFilePath)
	{
		OP_ENGINE_WARN("Initializing Resource Manager");
		s_ResourceManagerData.counter = 0;

		// Configure Root path
		s_ResourceManagerData.root = rootFilePath;
		OP_ENGINE_INFO("\tRoot path is : {0}", rootFilePath);

		std::filesystem::path assetPath = rootFilePath / "assets";
		std::filesystem::path modelPath = assetPath / "models";

		std::filesystem::path shaderPath = assetPath / "shaders";
		std::filesystem::path shaderSrcPath = shaderPath / "src";
		std::filesystem::path shaderIncludePath = shaderPath / "include";

		std::filesystem::path materialPath = assetPath / "materials";

		// Recursively go through all shader includes
		ResourceManager::LoadIncludeShaders(shaderIncludePath);
		ResourceManager::LoadShaderSources(shaderSrcPath);
		ResourceManager::CompileShaders();


		ResourceManager::LoadModels(modelPath);

		// Create generic resources
			// GENERIC MESHES
			uint32_t cube = Allocate("Cube");
			s_ResourceManagerData.Meshes[cube] = Cube::Create();

			uint32_t plane = Allocate("Plane");
			s_ResourceManagerData.Meshes[plane] = Plane::Create();

			uint32_t icosphere_s0_smooth = Allocate("Icosphere_s0_smooth");
			s_ResourceManagerData.Meshes[icosphere_s0_smooth] = Icosphere::Create(1.0f, 0, true);

			uint32_t icosphere_s1_smooth = Allocate("Icosphere_s1_smooth");
			s_ResourceManagerData.Meshes[icosphere_s1_smooth] = Icosphere::Create(1.0f, 1, true);

			uint32_t icosphere_s2_smooth = Allocate("Icosphere_s2_smooth");
			s_ResourceManagerData.Meshes[icosphere_s2_smooth] = Icosphere::Create(1.0f, 2, true);

			uint32_t icosphere_s3_smooth = Allocate("Icosphere_s3_smooth");
			s_ResourceManagerData.Meshes[icosphere_s3_smooth] = Icosphere::Create(1.0f, 3, true);

			uint32_t icosphere_s0_flat = Allocate("Icosphere_s0_flat");
			s_ResourceManagerData.Meshes[icosphere_s0_flat] = Icosphere::Create(1.0f, 0, false);

			uint32_t icosphere_s1_flat = Allocate("Icosphere_s1_flat");
			s_ResourceManagerData.Meshes[icosphere_s1_flat] = Icosphere::Create(1.0f, 1, false);

			uint32_t icosphere_s2_flat = Allocate("Icosphere_s2_flat");
			s_ResourceManagerData.Meshes[icosphere_s2_flat] = Icosphere::Create(1.0f, 2, false);

			uint32_t icosphere_s3_flat = Allocate("Icosphere_s3_flat");
			s_ResourceManagerData.Meshes[icosphere_s3_flat] = Icosphere::Create(1.0f, 3, false);



			uint32_t radialSphere_s0_smooth = Allocate("RadialSphere_s0_smooth");
			s_ResourceManagerData.Meshes[radialSphere_s0_smooth] = RadialSphere::Create(1.0f, 16, 16, true);

			uint32_t radialSphere_s1_smooth = Allocate("RadialSphere_s1_smooth");
			s_ResourceManagerData.Meshes[radialSphere_s1_smooth] = RadialSphere::Create(1.0f, 32, 32, true);

			uint32_t radialSphere_s2_smooth = Allocate("RadialSphere_s2_smooth");
			s_ResourceManagerData.Meshes[radialSphere_s2_smooth] = RadialSphere::Create(1.0f, 64, 64, true);

			uint32_t radialSphere_s3_smooth = Allocate("RadialSphere_s3_smooth");
			s_ResourceManagerData.Meshes[radialSphere_s3_smooth] = RadialSphere::Create(1.0f, 128, 128, true);

			uint32_t radialSphere_s0_flat = Allocate("RadialSphere_s0_flat");
			s_ResourceManagerData.Meshes[radialSphere_s0_flat] = RadialSphere::Create(1.0f, 16, 16, false);

			uint32_t radialSphere_s1_flat = Allocate("RadialSphere_s1_flat");
			s_ResourceManagerData.Meshes[radialSphere_s1_flat] = RadialSphere::Create(1.0f, 32, 32, false);

			uint32_t radialSphere_s2_flat = Allocate("RadialSphere_s2_flat");
			s_ResourceManagerData.Meshes[radialSphere_s2_flat] = RadialSphere::Create(1.0f, 64, 64, false);

			uint32_t radialSphere_s3_flat = Allocate("RadialSphere_s3_flat");
			s_ResourceManagerData.Meshes[radialSphere_s3_flat] = RadialSphere::Create(1.0f, 128, 128, false);



			// GENERIC TEXTURES

			// CONSIDER ENDIANNESS - CURRENT MACHINE IS LITTLE ENDIAN!!!
			uint32_t whiteTexture = Allocate("WhiteTexture");
			uint32_t whiteTextureData = 0xffffffff;
			s_ResourceManagerData.Textures[whiteTexture] = Texture2D::Create(1, 1, "WhiteTexture");
			s_ResourceManagerData.Textures[whiteTexture]->SetData(&whiteTextureData, sizeof(uint32_t));


			uint32_t blackTexture = Allocate("BlackTexture");
			uint32_t blackTextureData = 0xff000000;
			s_ResourceManagerData.Textures[blackTexture] = Texture2D::Create(1, 1, "BlackTexture");
			s_ResourceManagerData.Textures[blackTexture]->SetData(&blackTextureData, sizeof(uint32_t));

			uint32_t defaultNormalMap = Allocate("DefaultNormalMap");
			uint32_t defaultNormalMapData = 0xffff8080;
			s_ResourceManagerData.Textures[defaultNormalMap] = Texture2D::Create(1, 1, "DefaultNormalMap");
			s_ResourceManagerData.Textures[defaultNormalMap]->SetData(&defaultNormalMapData, sizeof(uint32_t));


			// LOAD MATERIALS
			LoadMaterials(materialPath);
		

		OP_ENGINE_WARN("Resource Manager has been inititalized!");
		return 0;
	}

	std::string ResourceManager::GetNameFromID(uint32_t id)
	{
		return s_ResourceManagerData.IDLookupTable[id];
	}

	int ResourceManager::AddMesh()
	{
		return 0;
	}

	Ref<Mesh> ResourceManager::GetMesh(std::string name)
	{
		uint32_t meshID = s_ResourceManagerData.StringLookupTable[name];
		return s_ResourceManagerData.Meshes[meshID];
	}

	const std::unordered_map<uint32_t, Ref<Mesh>>& ResourceManager::GetMeshMap()
	{
		return s_ResourceManagerData.Meshes;
	}

	int ResourceManager::LoadMaterials(std::filesystem::path materialsFilePath)
	{
		OP_ENGINE_WARN("\tLoading Materials");
		uint32_t id = Allocate("DefaultPbr");
		MaterialSpec defaultMaterialSpec;
		defaultMaterialSpec.MaterialName = "DefaultPbr";
		defaultMaterialSpec.Floats.push_back({ "roughness", 0.5f });
		defaultMaterialSpec.Floats.push_back({ "metalness", 0.1f });
		defaultMaterialSpec.Float3s.push_back({ "albedo", glm::vec3(1.0f,1.0f,1.0f) });
		defaultMaterialSpec.Textures.push_back({ "albedoMap",    GetTexture("WhiteTexture") });
		defaultMaterialSpec.Textures.push_back({ "roughnessMap", GetTexture("WhiteTexture") });
		defaultMaterialSpec.Textures.push_back({ "metalnessMap", GetTexture("WhiteTexture") });
		defaultMaterialSpec.Textures.push_back({ "aoMap",        GetTexture("WhiteTexture") });
		defaultMaterialSpec.Textures.push_back({ "normalMap", GetTexture("DefaultNormalMap") });
		defaultMaterialSpec.Textures.push_back({ "heightMap", GetTexture("BlackTexture") });
		Ref<Material> defaultMaterial = Material::Create(defaultMaterialSpec, GetShader("Main.glsl"));
		s_ResourceManagerData.Materials[id] = defaultMaterial;
		OP_ENGINE_WARN("\tMaterials have been loaded");

		return 0;
	}

	int ResourceManager::AddCubeMap(std::string filePath)
	{
		return 0;
	}

	int ResourceManager::AddPlainTexture(std::string filePath)
	{
		return 0;
	}

	int ResourceManager::AddAlbedoTexture(std::string filePath)
	{
		return 0;
	}

	int ResourceManager::AddMetalnessMap(std::string filePath)
	{
		return 0;
	}

	int ResourceManager::AddRoughnessMap(std::string filePath)
	{
		return 0;
	}

	int ResourceManager::AddNormalMap(std::string filePath)
	{
		return 0;
	}

	int ResourceManager::AddHeightMap(std::string filePath)
	{
		return 0;
	}

	int ResourceManager::AddShaderProgram(std::string filePath)
	{
		return 0;
	}

	int ResourceManager::LoadSceneResources(Ref<Scene> scene)
	{
		return 0;
	}

}