#pragma once
#include <Opium.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <Utils/AssimpGLMHelpers.h>
#include <Math/Math.h>


namespace OP
{
	namespace Utils
	{
		// Finds world coordinates of the frustum represented by
// the projection matrix
// We will use camera's projection matrix in this function
		static std::vector<glm::vec4> GetFrustumCornerCoordinatesWorldSpace(const glm::mat4& projViewMatrix)
		{
			const glm::mat4 inv = glm::inverse(projViewMatrix);

			std::vector<glm::vec4> frustumCoordinates;
			for (uint32_t x = 0; x < 2; x++)
			{
				for (uint32_t y = 0; y < 2; y++)
				{
					for (uint32_t z = 0; z < 2; z++)
					{
						const glm::vec4 ndcPoint = inv * glm::vec4(2.0f * x - 1.0f,
							2.0f * y - 1.0f,
							2.0f * z - 1.0f,
							1.0f);

						// Perspective division
						frustumCoordinates.push_back(ndcPoint / ndcPoint.w);
					}
				}
			}

			return frustumCoordinates;
		}

		// static std::vector<glm::vec4> GetFrustumCornerCoordinatesWorldSpace(const glm::mat4& projectionMatrix, const glm::mat4& view)
		// {
		//	return GetFrustomCornerCoordinatesWorldSpace(projectionMatrix * view);
		// }

		// We will get orthographic projection matrix which tightly overlaps the frustum part and
		// view matrix of the light (light will be on the center of frustum)
		static glm::mat4 GetLightLightSpaceMatrix(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const float nearPlane, const float farPlane, const glm::vec3& lightDir, const float zMult)
		{

			float left_, right_, bottom_, top_, near_, far_;


			Math::DecomposePerspectiveProj(projectionMatrix, left_, right_, bottom_, top_, near_, far_);

			float fovy = 2 * std::atan2(top_, near_);
			float aspectRatio = right_ / top_;

			glm::mat4 newProjectionMatrix = glm::perspective(fovy, aspectRatio, nearPlane, farPlane);

			std::vector<glm::vec4> frustumCornerCoordinates = GetFrustumCornerCoordinatesWorldSpace(newProjectionMatrix * viewMatrix);

			// Calculate view matrix
			glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
			for (const glm::vec4& coord : frustumCornerCoordinates)
			{
				center += glm::vec3(coord);
			}
			center /= frustumCornerCoordinates.size();

			glm::vec3 controlledLightDir = lightDir;
			if (controlledLightDir.x == 0.0 && controlledLightDir.z == 0.0)
			{
				controlledLightDir += glm::vec3(0.00001f, 0.0f, -0.00001f);
			}
			glm::mat4 lightView = glm::lookAt(center, center + controlledLightDir, glm::vec3(0.0f, 1.0f, 0.0f));

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();

			// Determine box bounds by traversing frustum coordinates
			for (const glm::vec4& coord : frustumCornerCoordinates)
			{
				const glm::vec4 coordAccordingToLight = lightView * coord;
				minX = std::min(minX, coordAccordingToLight.x);
				maxX = std::max(maxX, coordAccordingToLight.x);
				minY = std::min(minY, coordAccordingToLight.y);
				maxY = std::max(maxY, coordAccordingToLight.y);
				minZ = std::min(minZ, coordAccordingToLight.z);
				maxZ = std::max(maxZ, coordAccordingToLight.z);

				if (isnan(coordAccordingToLight.x) || isnan(coordAccordingToLight.y) || isnan(coordAccordingToLight.z))
					OP_ENGINE_INFO("isnan!");
			}

			// Tune parameter
			if (minZ < 0)
				minZ *= zMult;
			else
				minZ /= zMult;
			if (maxZ < 0)
				maxZ /= zMult;
			else
				maxZ *= zMult;

			const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

			return lightProjection * lightView;
		}

		// This function gives cascaded levels according to a distFactor.
		// The larger this factor is, further levels have much more far - near differences
		// If distributionFactor is 1, then far plane is divided equally.
		static std::vector<float> DistributeShadowCascadeLevels(uint32_t cascadeSize, float distFactor, float farPlane)
		{
			const uint32_t newPlaneSize = cascadeSize - 1;

			std::vector<float> levels;

			if (newPlaneSize == 0)
			{
				levels.push_back(farPlane);
				return levels;
			}

			float unitSize = farPlane / std::pow(cascadeSize, distFactor);
			levels.push_back(unitSize);

			float prevSum = 1.0f;
			for (uint32_t i = 2; i < newPlaneSize + 1; i++)
			{
				float newSum = std::pow(i, distFactor);
				levels.push_back(unitSize * (newSum));
				prevSum = newSum;
			}

			return levels;
		}


		// It gives light space matrices for each cascade
		static std::vector<glm::mat4> GetLightSpaceMatrices(const glm::mat4& projectionMatrix,
			const glm::mat4& viewMatrix,
			const std::vector<float>& cascadeLevels,
			const float nearPlane, const float farPlane,
			const glm::vec3& lightDir, const float zMult)
		{
			std::vector<glm::mat4> matrices;

			for (uint32_t i = 0; i < cascadeLevels.size(); i++)
			{
				if (i == 0)
				{
					matrices.push_back(GetLightLightSpaceMatrix(projectionMatrix, viewMatrix, nearPlane, cascadeLevels[i], lightDir, zMult));
				}
				else
				{
					matrices.push_back(GetLightLightSpaceMatrix(projectionMatrix, viewMatrix, cascadeLevels[i - 1], cascadeLevels[i], lightDir, zMult));
				}
			}

			if (cascadeLevels.size() > 1)
				matrices.push_back(GetLightLightSpaceMatrix(projectionMatrix, viewMatrix, cascadeLevels[cascadeLevels.size() - 1], farPlane, lightDir, zMult));

			return matrices;
		}
	}
}