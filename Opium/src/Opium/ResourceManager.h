#include <string>
#include <Scene/Scene.h>

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
		static int Init(std::string rootPath);

		// Mesh Related functions
		static int AddMesh();


		// Texture related functions
		static int AddCubeMap(std::string filePath);

		static int AddPlainTexture(std::string filePath);
		static int AddAlbedoTexture(std::string filePath);
		static int AddMetalnessMap(std::string filePath);
		static int AddRoughnessMap(std::string filePath);
		static int AddNormalMap(std::string filePath);
		static int AddHeightMap(std::string filePath);


		// Shaders related functions
		static int AddShaderProgram(std::string filePath);


		// Load scene related objects
		static int LoadSceneResources(Ref<Scene> scene);
	private:
		std::string m_RootPath;
	};


}