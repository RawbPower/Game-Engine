/*
	Perspective Camera

	Class used to set the projection of an perspective camera
*/

#pragma once

#include <glm/glm.hpp>

namespace ge {

	class PerspectiveCamera {
	public:
		PerspectiveCamera(float fov, float aspectRatio, float nearPlane, float farPlane);

		void SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane);

		void LookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
		void SetViewMatrix(const glm::mat4& view) { m_ViewMatrix = view; m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; }

		const glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;
	};
}
