#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>
#include <Animation/Bone.h>
#include <Geometry/Mesh.h>
#include <Utils/AssimpGLMHelpers.h>
#include <Animation/Animation.h>

#define MAX_BONES 100

namespace OP
{

	struct BoneMatrix
	{
		glm::mat4 mat;
	};

	// Bone matrices data
	struct BoneMatricesData
	{
		BoneMatrix BoneMatrices[MAX_BONES];

	};

	class AnimationHandler
	{
	public:
		AnimationHandler() 
		{
			m_CurrentAnimation = nullptr;
			m_DeltaTime = 0.0f;
		}

		AnimationHandler(Animation* anim)
		{
			m_CurrentTime = 0.0f;
			m_CurrentAnimation = anim;
			m_DeltaTime = 0.0f;

			for (int i = 0; i < MAX_BONES; i++)
			{
				m_FinalBoneMatrices.BoneMatrices[i].mat = glm::mat4(1.0f);
			}
		}

		void UpdateAnimation(float deltaTime)
		{
			m_DeltaTime = deltaTime;
			if (m_CurrentAnimation)
			{
				m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * deltaTime;
				m_CurrentTime = std::fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
				CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
			}
		}

		void PlayAnimation(Animation* anim)
		{
			m_CurrentAnimation = anim;
			m_CurrentTime = 0.0f;
		}

		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
		{
			std::string nodeName = node->name;
			glm::mat4 nodeTransform = glm::mat4(1.0f);//node->transformation;

			Bone* bone = m_CurrentAnimation->FindBone(nodeName);

			if (bone)
			{
				bone->Update(m_CurrentTime);
				nodeTransform = bone->GetLocalTransform();
			}

			glm::mat4 globalTransformation = node->globalTransInverse * parentTransform * nodeTransform;

			auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();

			if (boneInfoMap.find(nodeName) != boneInfoMap.end())
			{
				int index = boneInfoMap[nodeName].id;
				glm::mat4 offset = boneInfoMap[nodeName].offset;
				m_FinalBoneMatrices.BoneMatrices[index].mat = globalTransformation * offset * glm::inverse(node->globalTransInverse);
			}

			for (uint32_t i = 0; i < node->childrenCount; i++)
				CalculateBoneTransform(&node->children[i], globalTransformation);
		}

		BoneMatricesData& GetFinalBoneMatrices()
		{
			return m_FinalBoneMatrices;
		}

	private:
		BoneMatricesData m_FinalBoneMatrices;
		Animation* m_CurrentAnimation;
		float m_CurrentTime;
		float m_DeltaTime;
	};
}