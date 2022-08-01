#pragma once

#include <Geometry/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <unordered_map>

#include <string>

#include <Animation/Animation.h>
#include <Animation/AnimationHandler.h>

namespace OP
{
	// 




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
		void ProcessNode(aiNode* node, const aiScene* scene, ModelNode* currentNode, ModelNode* parentNode, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCounter);
		ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, aiNode* currentNode, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& BoneCounter);


		static Ref<Model> Create(aiNode* rootNode, const aiScene* scene);

		void UpdateAnimation(float deltaTime);
		void ChangeAnimation(int animIndex);
		void Draw();

		BoneMatricesData& GetFinalBoneMatrices();

	private:
		ModelNode* m_RootNode;

		std::vector<ModelMesh> m_ModelMeshes;

		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
		int m_BoneCounter = 0;

		AnimationHandler m_AnimationHandler;
		std::vector<Animation> m_Animations;
	};
}