#pragma once

#include <glm/glm.hpp>
#include <string>

#include <Opium/UUID.h>
#include <Renderer/Shader.h>
#include <Renderer/Texture.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>

namespace OP
{

	struct MaterialSpec
	{
		std::string MaterialName;

		float TilingFactor = 1.0f;
		float HeightFactor = 0.0f;
		bool ClipBorder = false;

		// Their indexes will be uniform locations in shader
		std::vector<std::pair<std::string, float>> Floats;
		std::vector<std::pair<std::string, glm::vec2>> Float2s;
		std::vector<std::pair<std::string, glm::vec3>> Float3s;
		std::vector<std::pair<std::string, glm::mat4>> Mat4s;
		std::vector<std::pair<std::string, int>> Ints;

		// Indexes of textures will be their slots in the shader
		std::vector<std::pair<std::string, Ref<Texture>>> Textures;
	};

	class Material
	{
	public:
		Material();

		Material(Ref<Shader> shader);

		Material(const MaterialSpec& spec, Ref<Shader> shader);

		void AssignShader(Ref<Shader> shader);

		// Binds the shader
		void Bind();

		// Gives material properties to shader
		// if used without an instance will give default
		// values
		void Use();

		
		void AddFloat(std::string name, float value);
		void AddFloat2(std::string name, glm::vec2 value);
		void AddFloat3(std::string name, glm::vec3 value);
		void AddMat4(std::string name, glm::mat4 value);
		void AddInt(std::string name, int value);

		void AddTexture(std::string name, Ref<Texture> tex);


		static Ref<Material> Create(const MaterialSpec& spec, Ref<Shader> shader);

		UUID m_ID;
		std::string m_Name;
		Ref<Shader> m_Shader;

		std::vector<std::pair<std::string, float>> m_Floats;
		std::vector<std::pair<std::string, glm::vec2>> m_Float2s;
		std::vector<std::pair<std::string, glm::vec3>> m_Float3s;
		std::vector<std::pair<std::string, glm::mat4>> m_Mat4s;
		std::vector<std::pair<std::string, int>> m_Ints;

		float m_TilingFactor = 1.0f;
		float m_HeightFactor = 0.0f;
		bool m_ClipBorder = false;
		// Indexes of textures will be their slots in the shader
		std::vector<std::pair<std::string, Ref<Texture>>> m_Textures;
	};

	struct MaterialInstance
	{
		MaterialInstance(Ref<Material> mat);
		void AssignValues();
		void ChangeMaterial(Ref<Material> newMat);

		void PopulateFields();

		Ref<Material> Mat;

		static Ref<MaterialInstance> Create(Ref<Material> mat);

		std::vector<std::pair<std::string, float>> Floats;
		std::vector<std::pair<std::string, glm::vec2>> Float2s;
		std::vector<std::pair<std::string, glm::vec3>> Float3s;
		std::vector<std::pair<std::string, glm::mat4>> Mat4s;
		std::vector<std::pair<std::string, int>> Ints;

		float TilingFactor = 1.0f;
		float HeightFactor = 0.0f;
		bool ClipBorder = false;
		// Indexes of textures will be their slots in the shader
		std::vector<std::pair<std::string, Ref<Texture>>> Textures;
	};
}