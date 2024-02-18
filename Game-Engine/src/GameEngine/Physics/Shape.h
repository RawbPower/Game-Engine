#pragma once

#include "GameEngine/Math/Vector.h"
#include "GameEngine/Math/Matrix.h"

namespace ge 
{
	class Shape
	{
	public:
		enum shapeType { SHAPE_SPHERE };
		virtual shapeType GetType() const = 0;

		virtual Vec3 GetCenterOfMass() const { return m_centerOfMass; }

		virtual float GetScale() const { return 1.0f; }

	protected:
		Vec3 m_centerOfMass;
	};

	class ShapeSphere : public Shape
	{
	public:
		ShapeSphere(float radius) : m_radius(radius) 
		{
			m_centerOfMass.Zero();
		}

		shapeType GetType() const override { return SHAPE_SPHERE; }

		float GetScale() const override{ return m_radius; }

		float m_radius;
	};
}
