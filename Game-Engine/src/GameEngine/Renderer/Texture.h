#pragma once

#include <string>

#include "GameEngine/Core/Core.h"

namespace ge {

	// Virtaul abstract base class for other texture classic to inherit from
	class Texture 
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
	};

	// 2D texture (still abstract, it needs to be implemented by specific renderer API's)
	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path);
	};
}
