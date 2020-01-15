#pragma once

#include "GameEngine/Renderer/PerspectiveCamera.h"
#include "GameEngine/Core/DeltaTime.h"

#include "GameEngine/Events/ApplicationEvent.h"
#include "GameEngine/Events/MouseEvent.h"

namespace ge {

	class PerspectiveCameraController
	{
	public:
		PerspectiveCameraController(float fov, float aspectRatio, float nearPlane, float farPlane);

		void OnUpdate(DeltaTime dt);
		void OnEvent(Event& e);

		PerspectiveCamera& GetCamera() { return m_Camera; }
		const PerspectiveCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }

		glm::vec3 GetCameraPosition() const { return m_CameraPosition; }
		void SetCameraPosition(glm::vec3 pos) { m_CameraPosition = pos; }

		glm::vec3 GetCameraFront() const { return m_CameraFront; }
		void SetCameraFront(glm::vec3 front) { m_CameraFront = front; }

		glm::vec3 GetCameraUp() const { return m_CameraUp; }
		void SetCameraUp(glm::vec3 level) { m_CameraUp = level; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		float m_Fov = 45.0f;
		PerspectiveCamera m_Camera;

		glm::vec3 m_CameraPosition = { -2.0f, -2.0f, 4.0f };
		glm::vec3 m_CameraFront = { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_CameraUp = { 0.0f, 1.0f, 0.0f };
		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 2.5f, m_CameraRotationSpeed = 180.0f;

		bool m_FirstMouse = true;
		float m_LastX = 640.0f, m_LastY = 360.0f;
		float m_Sensitivity = 0.05f;
	};

}
