#include <Precomp.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/OrthographicCameraController.h>
#include <Input/Input.h>
#include <Input/KeyCodes.h>

namespace Opium
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_Camera(-aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		  m_Rotation(rotation),
		  m_AspectRatio(aspectRatio),
		m_Bounds({ -aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel })
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		OP_PROFILE_FUNCTION();

		if (Input::IsKeyPressed(OP_KEY_A))
		{
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(OP_KEY_D))
		{
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(OP_KEY_W))
		{
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(OP_KEY_S))
		{
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
		}

		if (m_Rotation)
		{
			if (Input::IsKeyPressed(OP_KEY_Q))
			{
				m_CameraRotation += m_CameraRotationSpeed * ts;
			}
			if (Input::IsKeyPressed(OP_KEY_E))
			{
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			}

			m_Camera.SetRotation(m_CameraRotation);
		}


		m_Camera.SetPosition(m_CameraPosition);


	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		OP_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(OP_BIND_EVENT_FUNCTION(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(OP_BIND_EVENT_FUNCTION(OrthographicCameraController::OnWindowResized));

	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		CalculateView();
	}

	void OrthographicCameraController::CalculateView()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		OP_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		CalculateView();
		m_CameraTranslationSpeed = m_ZoomLevel;

		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		OP_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		CalculateView();
		return false;
	}

}