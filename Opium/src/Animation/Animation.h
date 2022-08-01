#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

#include <Animation/Bone.h>
#include <Geometry/Mesh.h>


#include <Utils/AssimpGLMHelpers.h>



namespace OP
{
	struct AssimpNodeData
	{
		glm::mat4 transformation;
		glm::mat4 globalTransInverse;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	class Animation
	{
	public:
		Animation() = default;

		Animation(aiAnimation* anim, const aiScene* scene, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCount)
		{
			m_Duration      = anim->mDuration;
			m_TicksPerSecond = anim->mTicksPerSecond;
			aiMatrix4x4 aiGlobalTransInverse = scene->mRootNode->mTransformation;
			aiGlobalTransInverse.Inverse();

			glm::mat4 globalTransInverse = AssimpGLMHelpers::ConvertMatrixToGLMFormat(aiGlobalTransInverse);
			ReadHierarchyData(m_RootNode, scene->mRootNode, globalTransInverse);
			ReadMissingBones(anim, boneInfoMap, boneCount);
		}

		~Animation() {}

		Bone* FindBone(const std::string& name)
		{
			auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
				[&](const Bone& Bone)
				{
					return Bone.GetBoneName() == name;
				});
			if (iter == m_Bones.end()) return nullptr;
			else return &(*iter);
		}

		inline float GetTicksPerSecond() { return m_TicksPerSecond; }

		inline float GetDuration() { return m_Duration; }

		inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

		inline const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap()
		{
			return m_BoneInfoMap;
		}

	private:
		void ReadMissingBones(const aiAnimation* anim, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCount)
		{
			int size = anim->mNumChannels;

			for (uint32_t i = 0; i < size; i++)
			{
				auto channel = anim->mChannels[i];
				std::string boneName = channel->mNodeName.data;

				if (boneInfoMap.find(boneName) == boneInfoMap.end())
				{
					boneInfoMap[boneName].id = boneCount;
					boneInfoMap[boneName].offset = glm::mat4(1.0f);
					boneCount++;
				}
				m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
			}

			m_BoneInfoMap = boneInfoMap;
		}

		void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src, glm::mat4 globalTransInverse)
		{
			dest.name = src->mName.data;
			dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
			dest.childrenCount = src->mNumChildren;
			dest.globalTransInverse = globalTransInverse;
			for (int i = 0; i < src->mNumChildren; i++)
			{
				AssimpNodeData newData;
				ReadHierarchyData(newData, src->mChildren[i], globalTransInverse);
				dest.children.push_back(newData);
			}
		}

	private:
		float m_Duration;
		int m_TicksPerSecond;
		std::vector<Bone> m_Bones;
		AssimpNodeData m_RootNode;
		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
	};
}