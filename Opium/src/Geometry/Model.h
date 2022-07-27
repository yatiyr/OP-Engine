#pragma once

#include <Geometry/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace OP
{

	struct ModelNode
	{
	    ModelNode* Parent;
		std::vector<Ref<Mesh>> Meshes;
		std::vector<ModelNode*> Children;
	};

	class Model
	{
	public:
		Model(aiNode* rootNode, const aiScene* scene);
		void ProcessNode(aiNode* node, const aiScene* scene, ModelNode* currentNode);
		Ref<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

	private:
		ModelNode* m_RootNode;
	};
}