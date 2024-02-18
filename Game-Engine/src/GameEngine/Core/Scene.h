#pragma once

#include "GameEngine/Physics/Body.h"
#include "GameEngine/Core/DeltaTime.h"

namespace ge
{
	class Scene
	{
	public:
		void Initialize();
		void Update(const DeltaTime dt);

		std::vector<Body> m_bodies;
	};
}