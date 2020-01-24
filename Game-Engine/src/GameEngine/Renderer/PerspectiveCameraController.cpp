#include "gepch.h"
#include "PerspectiveCameraController.h"

#include "GameEngine/Core/Input.h"
#include "GameEngine/Core/KeyCodes.h"

namespace ge {

	PerspectiveCameraController::PerspectiveCameraController(float fov, float aspectRatio, float nearPlane, float farPlane)
		: m_AspectRatio(aspectRatio), m_Fov(fov), m_Camera(fov, aspectRatio, nearPlane, farPlane)
	{

	}

	void PerspectiveCameraController::OnUpdate(DeltaTime dt)
	{
		m_Camera.LookAt(m_CameraPosition, m_CameraPosition + m_CameraFront, m_CameraUp);

		// Camera Movement
		if (Input::IsKeyPressed(GE_KEY_W))
			m_CameraPosition += (m_CameraTranslationSpeed * dt) * m_CameraFront;
		else if (Input::IsKeyPressed(GE_KEY_S))
			m_CameraPosition -= (m_CameraTranslationSpeed * dt) * m_CameraFront;

		if (Input::IsKeyPressed(GE_KEY_A))
			m_CameraPosition -= (m_CameraTranslationSpeed * dt) * glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
		else if (Input::IsKeyPressed(GE_KEY_D))
			m_CameraPosition += (m_CameraTranslationSpeed * dt) * glm::normalize(glm::cross(m_CameraFront, m_CameraUp));

	}

	void PerspectiveCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(GE_BIND_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
		dispatcher.Dispatch<MouseMovedEvent>(GE_BIND_EVENT_FN(PerspectiveCameraController::OnMouseMoved));
		dispatcher.Dispatch<WindowResizeEvent>(GE_BIND_EVENT_FN(PerspectiveCameraController::OnWindowResized));
	}

	bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_Fov -= e.GetYOffset();
		if (m_Fov <= 1.0f)
			m_Fov = 1.0f;
		else if (m_Fov >= 45.0f)
			m_Fov = 45.0f;
		m_Camera.SetProjection(m_Fov, m_AspectRatio, 0.1f, 1000.0f);
		return false;
	}

	bool PerspectiveCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		float xpos = e.GetX(), ypos = e.GetY();
		//GE_CORE_TRACE("({0},{1})", xpos, ypos);

		if (m_FirstMouse)
		{
			m_LastX = xpos;
			m_LastY = ypos;
			m_FirstMouse = false;
		}

		float xoffset = xpos - m_LastX;
		float yoffset = m_LastY - ypos;
		m_LastX = xpos, m_LastY = ypos;

		xoffset *= m_Sensitivity;
		yoffset *= m_Sensitivity;

		m_Yaw += xoffset;
		m_Pitch += yoffset;

		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		else if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		direction.y = sin(glm::radians(m_Pitch));
		direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_CameraFront = glm::normalize(direction);

		return false;
	}

	bool PerspectiveCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_ScreenHeight = e.GetHeight();
		m_ScreenWidth = e.GetWidth();
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(m_Fov, m_AspectRatio, 0.1f, 1000.0f);
		return false;
	}
}