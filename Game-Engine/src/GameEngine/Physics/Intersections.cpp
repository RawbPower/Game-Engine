#include "gepch.h"
#include "Intersections.h"

namespace ge
{
	bool Intersect(const Body* bodyA, const Body* bodyB)
	{
		const Vec3 ab = bodyB->m_position - bodyA->m_position;

		const ShapeSphere* sphereA = (const ShapeSphere*)bodyA->m_shape;
		const ShapeSphere* sphereB = (const ShapeSphere*)bodyB->m_shape;

		const float radiusAB = sphereA->m_radius + sphereB->m_radius;
		const float lengthSq = ab.GetMag2();
		if (lengthSq <= radiusAB * radiusAB)
		{
			return true;
		}

		return false;
	}
}