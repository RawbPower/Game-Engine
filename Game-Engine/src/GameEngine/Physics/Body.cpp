#include "gepch.h"
#include "Body.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ge
{
	Vec3 Body::GetCenterOfMassWorldSpace() const
	{
		const Vec3 centerOfMass = m_shape->GetCenterOfMass();
		const Vec3 pos = m_position + m_orientation.RotatePoint(centerOfMass);
		return pos;
	}

	Vec3 Body::GetCenterOfMassObjectSpace() const
	{
		const Vec3 centerOfMass = m_shape->GetCenterOfMass();
		return centerOfMass;
	}

	Vec3 Body::WorldSpaceToBodySpace(const Vec3& worldPt) const
	{
		Vec3 temp = worldPt - GetCenterOfMassWorldSpace();
		Quat inverseOrient = m_orientation.Inverse();
		Vec3 bodySpace = inverseOrient.RotatePoint(temp);
		return bodySpace;
	}

	Vec3 Body::BodySpaceToWorldSpace(const Vec3& bodyPt) const
	{
		Vec3 worldSpace = GetCenterOfMassWorldSpace() + m_orientation.RotatePoint(bodyPt);
		return worldSpace;
	}

	glm::mat4 Body::GetRenderTransform()
	{
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, (glm::vec3)m_position);
		float angle = m_orientation.GetAngle();
		Vec3 normal = m_orientation.GetNormal();
		transform = glm::rotate(transform, angle, (glm::vec3)normal);
		transform = glm::scale(transform, glm::vec3(m_shape->GetScale()));
		return transform;
	}
}