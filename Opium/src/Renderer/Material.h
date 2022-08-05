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
		std::unordered_map<std::string, float> Floats;
		std::unordered_map<std::string, glm::vec2> Float2s;
		std::unordered_map<std::string, glm::vec3> Float3s;
		std::unordered_map<std::string, glm::mat4> Mat4s;
		std::unordered_map<std::string, int> Ints;

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

		std::unordered_map<std::string, float> m_Floats;
		std::unordered_map<std::string, glm::vec2> m_Float2s;
		std::unordered_map<std::string, glm::vec3> m_Float3s;
		std::unordered_map<std::string, glm::mat4> m_Mat4s;
		std::unordered_map<std::string, int> m_Ints;

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

		std::unordered_map<std::string, float> Floats;
		std::unordered_map<std::string, glm::vec2> Float2s;
		std::unordered_map<std::string, glm::vec3> Float3s;
		std::unordered_map<std::string, glm::mat4> Mat4s;
		std::unordered_map<std::string, int> Ints;

		std::vector<std::pair<std::string, Ref<Texture>>> Textures;
	};
}