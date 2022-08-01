#pragma once

#include <glm/glm.hpp>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

namespace OP
{
	struct Material
	{

		Material(aiMaterial* material)
		{
			aiString name;
			material->Get(AI_MATKEY_NAME, name);
			Name = name.C_Str();

			aiColor3D albedoColor;
			material->Get(AI_MATKEY_COLOR_DIFFUSE, albedoColor);
			AlbedoColor.x = albedoColor.r;
			AlbedoColor.y = albedoColor.g;
			AlbedoColor.z = albedoColor.b;

			aiColor3D specularColor;
			material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
			SpecularColor.x = specularColor.r;
			SpecularColor.y = specularColor.g;
			SpecularColor.z = specularColor.b;

			aiColor3D ambientColor;
			material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
			AmbientColor.x = ambientColor.r;
			AmbientColor.y = ambientColor.g;
			AmbientColor.z = ambientColor.b;

			aiColor3D emissiveColor;
			material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
			EmissiveColor.x = emissiveColor.r;
			EmissiveColor.y = emissiveColor.g;
			EmissiveColor.z = emissiveColor.b;

			aiColor3D transparentColor;
			material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparentColor);
			TransparentColor.x = transparentColor.r;
			TransparentColor.y = transparentColor.g;
			TransparentColor.z = transparentColor.b;

			aiColor3D reflectiveColor;
			material->Get(AI_MATKEY_COLOR_REFLECTIVE, reflectiveColor);
			ReflectiveColor.x = reflectiveColor.r;
			ReflectiveColor.y = reflectiveColor.g;
			ReflectiveColor.z = reflectiveColor.b;

			material->Get(AI_MATKEY_REFLECTIVITY, Reflectivity);
			material->Get(AI_MATKEY_ENABLE_WIREFRAME, Wireframe);
			material->Get(AI_MATKEY_TWOSIDED, TwoSided);
			material->Get(AI_MATKEY_SHADING_MODEL, ShadingModel);
			material->Get(AI_MATKEY_BLEND_FUNC, BlendFunction);
			material->Get(AI_MATKEY_OPACITY, Opacity);
			material->Get(AI_MATKEY_SHININESS, Shininess);
			material->Get(AI_MATKEY_SHININESS_STRENGTH, ShininessStrength);
			material->Get(AI_MATKEY_REFRACTI, RefractI);

			auto getTexture = [&](aiMaterial* mat, aiTextureType type) -> std::string
			{

				aiString str;
				mat->Get(AI_MATKEY_TEXTURE(type, 0), str);
				return std::string(str.C_Str());
				/*std::string result;
				if (mat->GetTextureCount(type) > 0)
				{
					if (mat->GetTexture(type, 0, &str, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
					{
						result = str.C_Str();
					}
				}

				return result;*/
			};

			AlbedoTexName    = getTexture(material, aiTextureType_DIFFUSE);
			NormalTexName    = getTexture(material, aiTextureType_NORMALS);
			HeightTexName    = getTexture(material, aiTextureType_HEIGHT);
			EmissionTexName  = getTexture(material, aiTextureType_EMISSION_COLOR);
			MetalnessTexName = getTexture(material, aiTextureType_METALNESS);
			RoughnessTexName = getTexture(material, aiTextureType_DIFFUSE_ROUGHNESS);
			AOTexName        = getTexture(material, aiTextureType_AMBIENT_OCCLUSION);

			
		}

		std::string Name;
		glm::vec3 AlbedoColor = { 0.0f, 0.0f, 0.0f };
		glm::vec3 SpecularColor = { 0.0f, 0.0f, 0.0f };
		glm::vec3 AmbientColor = { 0.0f, 0.0f, 0.0f };
		glm::vec3 EmissiveColor = { 0.0f, 0.0f, 0.0f };
		glm::vec3 TransparentColor = { 0.0f, 0.0f, 0.0f };
		glm::vec3 ReflectiveColor = { 0.0f, 0.0f, 0.0f };

		float Reflectivity = 0.0f;
		bool Wireframe = false;
		bool TwoSided = false;
		int ShadingModel = 0;
		int BlendFunction = 0;
		float Opacity = 1.0f;
		float Shininess = 0.0f;
		float ShininessStrength = 1.0f;
		float RefractI = 1.0f;

		std::string AlbedoTexName;
		std::string NormalTexName;
		std::string HeightTexName;
		std::string EmissionTexName;
		std::string MetalnessTexName;
		std::string RoughnessTexName;
		std::string AOTexName;
	};
}