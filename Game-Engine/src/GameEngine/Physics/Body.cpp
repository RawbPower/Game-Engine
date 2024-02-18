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

	void Body::ApplyImpulseLinear(const Vec3& impulse)
	{
		if (m_invMass == 0.0f)
		{
			return;
		}

		// p = mv
		// dp = m dv
		// => dv = dp / m
		m_linearVelocity += impulse * m_invMass;
	}

	glm::mat4 Body::GetRenderTransform()
	{
		glm::mat4 transform = glm::mat4(1.0f);
		glm::vec3 pos = glm::vec3(m_position.x, m_position.z, -m_position.y);
		transform = glm::translate(transform, pos);
		float angle = m_orientation.GetAngle();
		Vec3 normal = m_orientation.GetNormal();
		normal = Vec3(normal.x, normal.z, -normal.y);
		transform = glm::rotate(transform, angle, (glm::vec3)normal);
		transform = glm::scale(transform, glm::vec3(m_shape->GetScale()));
		return transform;
	}
}