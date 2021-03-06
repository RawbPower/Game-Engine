/*
	Orthographic Camera Controller

	Class used to control a orthographic camera
*/

#include "gepch.h"
#include "OrthographicCameraController.h"

#include "GameEngine/Core/Input.h"
#include "GameEngine/Core/KeyCodes.h"

namespace ge {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel,-m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{

	}

	void OrthographicCameraController::OnUpdate(DeltaTime dt)
	{
		// Camera Movement
		if (Input::IsKeyPressed(GE_KEY_A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * dt;
		else if (Input::IsKeyPressed(GE_KEY_D))
			m_CameraPosition.x += m_CameraTranslationSpeed * dt;

		if (Input::IsKeyPressed(GE_KEY_S))
			m_CameraPosition.y -= m_CameraTranslationSpeed * dt;
		else if (Input::IsKeyPressed(GE_KEY_W))
			m_CameraPosition.y += m_CameraTranslationSpeed * dt;

		// Camera Rotation
		if (m_Rotation)
		{
			// For some reason the positive rotation is clockwise
			if (Input::IsKeyPressed(GE_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * dt;

			if (Input::IsKeyPressed(GE_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * dt;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(GE_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(GE_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}