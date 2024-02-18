#include "gepch.h"
#include "Scene.h"

namespace ge 
{
	void Scene::Initialize()
	{
		Body body;
		body.m_position = Vec3(0, 0, 0);
		body.m_orientation = Quat(0,0,0,1);
		body.m_shape = new ShapeSphere(1.0f);
		m_bodies.push_back(body);
	}

	void Scene::Update(DeltaTime dt)
	{
	}
}