#include <Precomp.h>
#include <Math/Math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace OP::Math
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale)
	{
		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())
			)
		{
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		outTranslation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		for (length_t i = 0; i < 3; ++i)
		{
			for (length_t j = 0; j < 3; ++j)
			{
				Row[i][j] = LocalMatrix[i][j];
			}
		}

		outScale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		outScale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		outScale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		outRotation.y = asin(-Row[0][2]);
		if (cos(outRotation.y) != 0)
		{
			outRotation.x = atan2(Row[1][2], Row[2][2]);
			outRotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			outRotation.x = atan2(-Row[2][0], Row[1][1]);
			outRotation.z = 0;
		}


		return true;
	}

	// GLM matrices are column major!
	bool DecomposePerspectiveProj(const glm::mat4& perspectiveProjection, float& outLeft, float& outRight,
		float& outBottom, float& outTop,
		float& outNear, float& outFar)
	{

		outNear   = perspectiveProjection[4][3] / (perspectiveProjection[3][3] - 1);
		outFar    = perspectiveProjection[4][3] / (perspectiveProjection[3][3] + 1);

		outLeft   = outNear * (perspectiveProjection[3][1] - 1) / perspectiveProjection[1][1];
		outRight  = outNear * (perspectiveProjection[3][1] + 1) / perspectiveProjection[1][1];

		outBottom = outNear * (perspectiveProjection[3][2] - 1) / perspectiveProjection[2][2];
		outTop    = outNear * (perspectiveProjection[3][2] + 1) / perspectiveProjection[2][2];

		return true;
	}

}