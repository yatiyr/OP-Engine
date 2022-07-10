#pragma once

#include <glm/glm.hpp>

namespace OP
{
	namespace Math
	{
		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);
		bool DecomposePerspectiveProj(const glm::mat4& perspectiveProjection, float& outLeft, float& outRight,
																			  float& outBottom, float& outTop,
																			  float& outNear, float& outFar);
	}
}