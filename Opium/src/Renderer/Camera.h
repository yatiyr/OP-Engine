#pragma once

#include <glm/glm.hpp>

namespace OP
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() {}

		const glm::mat4& GetProjection() const { return m_Projection; }

		void SetPerspective() {}
		void SetOrtho() {}

		float GetFarClip() { return m_FarClip; }
		float GetNearClip() { return m_NearClip; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		float m_FarClip = 1000.0f;
		float m_NearClip = 0.1f;
	};
}