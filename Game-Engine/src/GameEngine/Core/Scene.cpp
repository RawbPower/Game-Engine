#include "gepch.h"
#include "Scene.h"
#include "GameEngine/Physics/Intersections.h"

namespace ge 
{
	void Scene::Initialize()
	{
		Body body;
		body.m_position = Vec3(0, 0, 12);
		//body.m_orientation = Quat(0,0,0,1);
		body.m_orientation = Quat(Vec3(0, 0, 1), 0);
		body.m_invMass = 1.0f;
		body.m_shape = new ShapeSphere(1.0f);
		m_bodies.push_back(body);

		// Add a "ground" sphere that won't fall under the influence of gravity
		body.m_position = Vec3(0, 0, -1000);
		body.m_orientation = Quat(Vec3(0, 0, 1), 0);
		body.m_invMass = 0.0f;
		body.m_shape = new ShapeSphere(1000.0f);
		m_bodies.push_back(body);
	}

	void Scene::Update(DeltaTime dt)
	{
		for (int i = 0; i < m_bodies.size(); i++)
		{
			Body* body = &m_bodies[i];

			// Gravity needs to be an impulse
			// F = dp/dt => dp = F * dt
			// F = mg
			float mass = 1.0f / body->m_invMass;
			Vec3 impulseGravity = Vec3(0, 0, -9.8) * mass * dt;
			body->ApplyImpulseLinear(impulseGravity);
		}

		// Check for collisions with bodies
		for (int i = 0; i < m_bodies.size(); i++)
		{
			for (int j = i+1; j < m_bodies.size(); j++)
			{
				Body* bodyA = &m_bodies[i];
				Body* bodyB = &m_bodies[j];

				// Skip body pairs with infinite mass
				if (bodyA->m_invMass == 0.0f && bodyB->m_invMass == 0.0f)
				{
					continue;
				}

				if (Intersect(bodyA, bodyB))
				{
					bodyA->m_linearVelocity.Zero();
					bodyB->m_linearVelocity.Zero();
				}
			}
		}

		for (int i = 0; i < m_bodies.size(); i++)
		{
			// Position update
			m_bodies[i].m_position += m_bodies[i].m_linearVelocity * dt;
		}
	}
}