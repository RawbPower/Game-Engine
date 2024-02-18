#pragma once

#include "GameEngine/Math/Vector.h"
#include "GameEngine/Math/Quat.h"
#include "Shape.h"

#include <glm/glm.hpp>

namespace ge
{
	class Body
	{
	public:
		Vec3 m_position;
		Quat m_orientation;
		Shape* m_shape;

		// Body Space - Origin an COM
		// Object Space - Origin at geomatrix center

		Vec3 GetCenterOfMassWorldSpace() const;
		Vec3 GetCenterOfMassObjectSpace() const;

		Vec3 WorldSpaceToBodySpace(const Vec3& worldPt) const;
		Vec3 BodySpaceToWorldSpace(const Vec3& bodyPt) const;

		glm::mat4 GetRenderTransform();
	};
}
