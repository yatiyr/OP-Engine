#include <Precomp.h>

#include <Geometry/Model.h>

namespace OP
{
	Model::Model(aiNode* rootNode, const aiScene* scene)
	{
		m_RootNode = new ModelNode();
		
		ProcessNode(rootNode, scene, m_RootNode, nullptr);
	}
	void Model::ProcessNode(aiNode* node, const aiScene* scene, ModelNode* currentNode, ModelNode* parentNode)
	{
		currentNode->Name = node->mName.C_Str();
		currentNode->Parent = parentNode;
		for (uint32_t i=0; i<node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			currentNode->Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i=0; i<node->mNumChildren; i++)
		{
			ModelNode* childNode = new ModelNode();
			currentNode->Children.push_back(childNode);
			ProcessNode(node->mChildren[i], scene, childNode, currentNode);
		}
	}

	ModelMesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		ModelMesh modelMesh;
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			modelMesh.Material = material;
		}

		modelMesh.Mesh = Mesh::Create(mesh, scene);
		modelMesh.Name = mesh->mName.C_Str();

		return modelMesh;
	}

	Ref<Model> Model::Create(aiNode* rootNode, const aiScene* scene)
	{
		return std::make_shared<Model>(rootNode, scene);
	}

}