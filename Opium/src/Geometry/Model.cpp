#include <Precomp.h>

#include <Geometry/Model.h>
#include <Renderer/Material.h>

namespace OP
{
	Model::Model(aiNode* rootNode, const aiScene* scene)
	{
		m_RootNode = new ModelNode();
		
		uint32_t animNumber = scene->mNumAnimations;

		ProcessNode(rootNode, scene, m_RootNode, nullptr, m_BoneInfoMap, m_BoneCounter);


		for (uint32_t i = 0; i < animNumber; i++)
		{
			Animation anim(scene->mAnimations[i], scene, m_BoneInfoMap, m_BoneCounter);
			m_Animations.push_back(anim);
		}

		if (animNumber > 0)
			m_AnimationHandler = AnimationHandler(&m_Animations[0]);
	}
	void Model::ProcessNode(aiNode* node, const aiScene* scene, ModelNode* currentNode, ModelNode* parentNode, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCounter)
	{
		currentNode->Name = node->mName.C_Str();
		currentNode->Parent = parentNode;
		currentNode->LocalTransformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);

		for (uint32_t i=0; i<node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ModelMesh modelMesh = ProcessMesh(mesh, scene, node, boneInfoMap, boneCounter);
			currentNode->Meshes.push_back(modelMesh);
			m_ModelMeshes.push_back(modelMesh);
		}

		for (uint32_t i=0; i<node->mNumChildren; i++)
		{
			ModelNode* childNode = new ModelNode();
			currentNode->Children.push_back(childNode);
			ProcessNode(node->mChildren[i], scene, childNode, currentNode, boneInfoMap, boneCounter);
		}
	}

	ModelMesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, aiNode* currentNode, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCounter)
	{
		ModelMesh modelMesh;
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			modelMesh.Material = material;

			auto getTexture = [&](aiMaterial* mat, aiTextureType type, std::string typeName) -> void {

				for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
				{
					aiString str;
					mat->GetTexture(type, i, &str);
					std::string textureName = str.C_Str();
					OP_ENGINE_TRACE("Texture Name: {0}", textureName);
					OP_ENGINE_TRACE("Texture Type: {0}", type);
				}
			};

			Material readMaterial(material);

			aiString roughness;
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, 0), roughness);
			

			getTexture(material, aiTextureType_DIFFUSE, "diffuse");
			getTexture(material, aiTextureType_NORMALS, "normals");
			getTexture(material, aiTextureType_SPECULAR, "specular");
			getTexture(material, aiTextureType_AMBIENT, "unknown");

			getTexture(material, aiTextureType_BASE_COLOR, "albedo");
			getTexture(material, aiTextureType_NORMAL_CAMERA, "normalMap");
			getTexture(material, aiTextureType_HEIGHT, "height");
			getTexture(material, aiTextureType_EMISSION_COLOR, "emission");
			getTexture(material, aiTextureType_METALNESS, "metalness");
			getTexture(material, aiTextureType_DIFFUSE_ROUGHNESS, "roughness");
			getTexture(material, aiTextureType_AMBIENT_OCCLUSION, "ao");
		}

		modelMesh.Mesh = Mesh::Create(mesh, scene, currentNode, boneInfoMap, boneCounter);
		modelMesh.Name = mesh->mName.C_Str();

		return modelMesh;
	}

	Ref<Model> Model::Create(aiNode* rootNode, const aiScene* scene)
	{
		return std::make_shared<Model>(rootNode, scene);
	}

	void Model::UpdateAnimation(float deltaTime)
	{
		m_AnimationHandler.UpdateAnimation(deltaTime);
	}

	void Model::ChangeAnimation(int animIndex)
	{
		if(animIndex >= 0 && animIndex < m_Animations.size())
			m_AnimationHandler.PlayAnimation(&m_Animations[animIndex]);
	}

	void Model::Draw()
	{
		for (auto& element : m_ModelMeshes)
		{
			element.Mesh->Draw();
		}
	}

	BoneMatricesData& Model::GetFinalBoneMatrices()
	{
		return m_AnimationHandler.GetFinalBoneMatrices();
	}

}