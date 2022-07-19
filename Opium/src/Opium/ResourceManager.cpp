#include <Precomp.h>
#include <Opium/ResourceManager.h>

#include <Geometry/Mesh.h>
#include <Geometry/Icosphere.h>
#include <Geometry/Plane.h>
#include <Geometry/RadialSphere.h>
#include <Geometry/Cube.h>

#include <Renderer/Texture.h>

#include <Renderer/Shader.h>

namespace OP
{


	struct ResourceManagerData
	{
		std::unordered_map<uint32_t, Ref<Mesh>> Meshes;

		std::unordered_map<uint32_t, Ref<Texture>> CubeMaps;
		std::unordered_map<uint32_t, Ref<Texture>> PlainTextures;
		std::unordered_map<uint32_t, Ref<Texture>> AlbedoTextures;
		std::unordered_map<uint32_t, Ref<Texture>> MetalnessTextures;
		std::unordered_map<uint32_t, Ref<Texture>> RoughnessTextures;
		std::unordered_map<uint32_t, Ref<Texture>> NormalTextures;
		std::unordered_map<uint32_t, Ref<Texture>> HeightTextures;

		std::unordered_map<uint32_t, Ref<Shader>> Shaders;
		std::unordered_map<std::string, std::string> ShaderSources;
		std::unordered_map<std::string, std::string> IncludeShaderSources;


		std::unordered_map<std::string, uint32_t> StringLookupTable;

		uint32_t counter = 0;

		std::filesystem::path root;
	} s_ResourceManagerData;


	static uint32_t Allocate(std::string name)
	{
		s_ResourceManagerData.StringLookupTable[name] = s_ResourceManagerData.counter;
		s_ResourceManagerData.counter++;

		return s_ResourceManagerData.counter;
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
					s_ResourceManagerData.Shaders[s_ResourceManagerData.counter] = Shader::Create(key, sources[0], sources[1]);
					break;
				// Vertex Fragment Geometry
				case 3:
					s_ResourceManagerData.StringLookupTable[key] = s_ResourceManagerData.counter;
					s_ResourceManagerData.Shaders[s_ResourceManagerData.counter] = Shader::Create(key, sources[0], sources[1], sources[2]);
					break;
			}

			s_ResourceManagerData.counter++;

		}

		OP_ENGINE_WARN("\tShaders have been compiled!");
		return 0;
	}

	Ref<Shader> ResourceManager::GetShader(std::string name)
	{
		uint32_t shaderHandle =  s_ResourceManagerData.StringLookupTable[name];
		return s_ResourceManagerData.Shaders[shaderHandle];
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

		std::filesystem::path shaderPath = assetPath / "shaders";
		std::filesystem::path shaderSrcPath = shaderPath / "src";
		std::filesystem::path shaderIncludePath = shaderPath / "include";
		// Recursively go through all shader includes
		ResourceManager::LoadIncludeShaders(shaderIncludePath);
		ResourceManager::LoadShaderSources(shaderSrcPath);
		ResourceManager::CompileShaders();

		// Create generic resources
			// GENERIC MESHES
			uint32_t icosphere = Allocate("Icosphere_1.0_4_smooth");
			s_ResourceManagerData.Meshes[icosphere] = Icosphere::Create(1.0f, 4, true);

			uint32_t icosphere2 = Allocate("Icosphere_1.0_0_flat");
			s_ResourceManagerData.Meshes[icosphere2] = Icosphere::Create(1.0f, 0, false);

			uint32_t icosphere3 = Allocate("Icosphere_1.0_1_flat");
			s_ResourceManagerData.Meshes[icosphere3] = Icosphere::Create(1.0f, 1, false);

			uint32_t cube = Allocate("Cube");
			s_ResourceManagerData.Meshes[cube] = Cube::Create();

			uint32_t plane = Allocate("Plane");
			s_ResourceManagerData.Meshes[plane] = Plane::Create();

			// GENERIC TEXTURES
			uint32_t whiteTexture = Allocate("WhiteTexture");
			uint32_t whiteTextureData = 0xffffffff;
			s_ResourceManagerData.PlainTextures[whiteTexture] = Texture2D::Create(1, 1);
			s_ResourceManagerData.PlainTextures[whiteTexture]->SetData(&whiteTextureData, sizeof(uint32_t));
		

		OP_ENGINE_WARN("Resource Manager has been inititalized!");
		return 0;
	}

	int ResourceManager::AddMesh()
	{
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