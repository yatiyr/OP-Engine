#include <Precomp.h>

#include <Geometry/Model.h>

namespace OP
{
	Model::Model(aiNode* rootNode, const aiScene* scene)
	{
		m_RootNode = new ModelNode();
		
		ProcessNode(rootNode, scene, m_RootNode);
	}
	void Model::ProcessNode(aiNode* node, const aiScene* scene, ModelNode* currentNode)
	{
		for (uint32_t i=0; i<node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			currentNode->Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i=0; i<node->mNumChildren; i++)
		{
			ModelNode* childNode = new ModelNode();
			currentNode->Children.push_back(childNode);
			ProcessNode(node->mChildren[i], scene, childNode);
		}
	}

	Ref<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		return Mesh::Create(mesh, scene);
	}

}