#pragma once

#include <Geometry/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>

namespace OP
{
	// These mesh structs will be turned into entities
	// in the editor and games
	struct ModelMesh
	{
		std::string Name;
		Ref<Mesh> Mesh;
		aiMaterial* Material;
	};

	struct ModelNode
	{
		std::string Name;
	    ModelNode* Parent;
		std::vector<ModelMesh> Meshes;
		std::vector<ModelNode*> Children;
	};

	class Model
	{
	public:
		Model(aiNode* rootNode, const aiScene* scene);
		void ProcessNode(aiNode* node, const aiScene* scene, ModelNode* currentNode, ModelNode* parentNode);
		ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

		static Ref<Model> Create(aiNode* rootNode, const aiScene* scene);

	private:
		ModelNode* m_RootNode;
	};
}