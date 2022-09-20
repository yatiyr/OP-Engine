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

#include <Utils/AssimpGLMHelpers.h>

namespace OP
{
	struct KeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};

	struct KeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};

	struct KeyScale
	{
		glm::vec3 scale;
		float timeStamp;
	};

	class Bone
	{
	public:
		Bone(const std::string& name, int ID, const aiNodeAnim* channel)
		: m_Name(name), m_ID(ID), m_LocalTransform(1.0f) 
		{

			m_NumPositions = channel->mNumPositionKeys;
			for (uint32_t positionIndex = 0; positionIndex < m_NumPositions; positionIndex++)
			{
				aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
				float timeStamp = channel->mPositionKeys[positionIndex].mTime;
				KeyPosition data;
				data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
				data.timeStamp = timeStamp;
				m_Positions.push_back(data);
			}

			m_NumRotations = channel->mNumRotationKeys;
			for (uint32_t rotationIndex = 0; rotationIndex < m_NumRotations; rotationIndex++)
			{
				aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
				float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
				KeyRotation data;
				data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
				data.timeStamp = timeStamp;
				m_Rotations.push_back(data);
			}

			m_NumScalings = channel->mNumScalingKeys;
			for (uint32_t keyIndex = 0; keyIndex < m_NumScalings; keyIndex++)
			{
				aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
				float timeStamp = channel->mScalingKeys[keyIndex].mTime;
				KeyScale data;
				data.scale = AssimpGLMHelpers::GetGLMVec(scale);
				data.timeStamp = timeStamp;
				m_Scales.push_back(data);
			}
		}

		/* Interpolate translation rotation and scale by looking at the values in keys and current time */
		void Update(float animationTime)
		{
			glm::mat4 translation = InterpolatePosition(animationTime);
			glm::mat4 rotation    = InterpolateRotation(animationTime);
			glm::mat4 scale       = InterpolateScaling(animationTime);
			m_LocalTransform = translation * rotation * scale;
		}

		glm::mat4 GetLocalTransform() { return m_LocalTransform; }
		const std::string& GetBoneName() const { return m_Name; }

		int GetBoneID() { return m_ID; }

		/* Get current index of position according to the time */
		int GetPositionIndex(float animationTime)
		{
			for (int index = 0; index < m_NumPositions - 1; index++)
			{
				if (animationTime < m_Positions[index + 1].timeStamp)
					return index;
			}

			return 0;
		}

		/* Get current index of rotation according to the time */
		int GetRotationIndex(float animationTime)
		{
			for (int index = 0; index < m_NumRotations - 1; index++)
			{
				if (animationTime < m_Rotations[index + 1].timeStamp)
					return index;
			}

			return 0;
		}

		/* Get current index of scaling according to the time */
		int GetScalingIndex(float animationTime)
		{
			for (int index = 0; index < m_NumScalings - 1; index++)
			{
				if (animationTime < m_Scales[index + 1].timeStamp)
					return index;
			}

			return 0;
		}

	private:

		/* Get normalized value for linear and spherical linear interpolations */
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
		{
			float scaleFactor = 0.0f;
			float midWayLength = animationTime - lastTimeStamp;
			float framesDiff = nextTimeStamp - lastTimeStamp;
			scaleFactor = midWayLength / framesDiff;
			return scaleFactor;
		}

		/* Interpolate position by fetching the index according to current time */
		glm::mat4 InterpolatePosition(float animationTime)
		{
			if (m_NumPositions == 1)
				return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

			int p0Index = GetPositionIndex(animationTime);
			int p1Index = p0Index + 1;
			float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
											   m_Positions[p1Index].timeStamp,
											   animationTime);

			glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position,
											   m_Positions[p1Index].position,
											   scaleFactor);

			return glm::translate(glm::mat4(1.0f), finalPosition);
		}

		/* Interpolate rotation by fetching the index according to current time */
		glm::mat4 InterpolateRotation(float animationTime)
		{
			if (m_NumRotations == 1)
			{
				auto rotation = glm::normalize(m_Rotations[0].orientation);
				return glm::toMat4(rotation);
			}

			int p0Index = GetRotationIndex(animationTime);
			int p1Index = p0Index + 1;

			float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
											   m_Rotations[p1Index].timeStamp,
											   animationTime);

			glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation,
												 m_Rotations[p1Index].orientation,
												 scaleFactor);

			return glm::toMat4(finalRotation);
		}

		/* Interpolate scaling by fetching the index according to current time */
		glm::mat4 InterpolateScaling(float animationTime)
		{
			if (m_NumScalings == 1)
				return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

			int p0Index = GetScalingIndex(animationTime);
			int p1Index = p0Index + 1;

			float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
											   m_Scales[p1Index].timeStamp,
											   animationTime);

			glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale,
											m_Scales[p1Index].scale,
											scaleFactor);

			return glm::scale(glm::mat4(1.0f), finalScale);
		}

	private:
		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;

		int m_NumPositions;
		int m_NumRotations;
		int m_NumScalings;

		glm::mat4 m_LocalTransform;
		std::string m_Name;
		int m_ID;
	};
}