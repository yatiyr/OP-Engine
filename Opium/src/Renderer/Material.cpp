#include <Precomp.h>
#include "Material.h"

namespace OP
{
	Material::Material()
	{

	}

	Material::Material(Ref<Shader> shader)
	{
		m_Shader = shader;
	}

	Material::Material(const MaterialSpec& spec, Ref<Shader> shader)
	{
		m_Name     = spec.MaterialName;
		m_Shader   = shader;

		m_Floats   = spec.Floats;
		m_Float2s  = spec.Float2s;
		m_Float3s  = spec.Float3s;
		m_Mat4s    = spec.Mat4s;
		m_Ints     = spec.Ints;
		m_Textures = spec.Textures;


	}


	void Material::AssignShader(Ref<Shader> shader)
	{
		m_Shader = shader;
	}

	void Material::Bind()
	{
		m_Shader->Bind();

		// It will include draw commands as well
	}

	void Material::AddFloat(std::string name, float value)
	{
		m_Floats.push_back({ name,value });
	}

	void Material::AddFloat2(std::string name, glm::vec2 value)
	{
		m_Float2s.push_back({ name, value });
	}

	void Material::AddFloat3(std::string name, glm::vec3 value)
	{
		m_Float3s.push_back({ name, value });
	}

	void Material::AddMat4(std::string name, glm::mat4 value)
	{
		m_Mat4s.push_back({ name, value });
	}

	void Material::AddInt(std::string name, int value)
	{
		m_Ints.push_back({ name, value });
	}

	void Material::AddTexture(std::string name, Ref<Texture> tex)
	{
		m_Textures.push_back({ name, tex });
	}

	Ref<Material> Material::Create(const MaterialSpec& spec, Ref<Shader> shader)
	{
		return std::make_shared<Material>(spec, shader);
	}


	MaterialInstance::MaterialInstance(Ref<Material> mat)
	{
		Mat = mat;
		PopulateFields();
	}

	void MaterialInstance::AssignValues()
	{

		uint32_t currentLoc = 0;

		for (auto& [name, val] : Floats)
		{
			Mat->m_Shader->SetFloat(currentLoc, val);
			currentLoc++;
		}

		for (auto& [name, val] : Float2s)
		{
			Mat->m_Shader->SetFloat2(currentLoc, val);
			currentLoc++;
		}

		for (auto& [name, val] : Float3s)
		{
			Mat->m_Shader->SetFloat3(currentLoc, val);
			currentLoc++;
		}

		for (auto& [name, val] : Mat4s)
		{
			Mat->m_Shader->SetMat4(currentLoc, val);
			currentLoc++;
		}

		for (auto& [name, val] : Ints)
		{
			Mat->m_Shader->SetInt(currentLoc, val);
			currentLoc;
		}
	}

	void MaterialInstance::ChangeMaterial(Ref<Material> newMat)
	{
		Mat = newMat;
		PopulateFields();
	}

	void MaterialInstance::PopulateFields()
	{
		Floats = Mat->m_Floats;
		Float2s = Mat->m_Float2s;
		Float3s = Mat->m_Float3s;
		Mat4s = Mat->m_Mat4s;
		Ints = Mat->m_Ints;
	}

	Ref<MaterialInstance> MaterialInstance::Create(Ref<Material> mat)
	{
		return std::make_shared<MaterialInstance>(mat);
	}


}