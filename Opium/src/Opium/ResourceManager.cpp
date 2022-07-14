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

		std::unordered_map<std::string, uint32_t> StringLookupTable;

		uint32_t counter = 0;
	} s_ResourceManagerData;


	static uint32_t Allocate(std::string name)
	{
		s_ResourceManagerData.StringLookupTable[name] = s_ResourceManagerData.counter;
		s_ResourceManagerData.counter++;

		return s_ResourceManagerData.counter;
	}
	

	// Reads the structure in root file path and loads the resources
	int ResourceManager::Init(std::string rootFilePath="")
	{
		s_ResourceManagerData.counter = 0;

		// First create generic resources
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

			// GENERIC SHADERS
			uint32_t mainShader = Allocate("MainShader");
			s_ResourceManagerData.Shaders[mainShader] = Shader::Create("assets/shaders/Pbr/Main.glsl");

			uint32_t depthShader = Allocate("ShadowMapDepthShader");
			s_ResourceManagerData.Shaders[depthShader] = Shader::Create("assets/shaders/Pbr/DirSpotShadowMapping.glsl");

			uint32_t shadowMapDirSpotBlur = Allocate("ShadowMapDirSpotBlur");
			s_ResourceManagerData.Shaders[shadowMapDirSpotBlur] = Shader::Create("assets/shaders/Pbr/DirSpotShadowMappingBlur.glsl");


			// GENERIC TEXTURES
			uint32_t whiteTexture = Allocate("WhiteTexture");
			uint32_t whiteTextureData = 0xffffffff;
			s_ResourceManagerData.PlainTextures[whiteTexture] = Texture2D::Create(1, 1);
			s_ResourceManagerData.PlainTextures[whiteTexture]->SetData(&whiteTextureData, sizeof(uint32_t));
		
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