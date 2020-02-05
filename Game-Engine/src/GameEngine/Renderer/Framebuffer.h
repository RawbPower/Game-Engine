/*
	Framebuffer

	Abstract class for framebuffer inherited by classes for each Renderering API
*/

#pragma once

namespace ge {

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Rescale(const uint32_t width, const uint32_t height) const = 0;
		virtual void BindTexture() const = 0;
		virtual void AttachCubemapTexture(uint32_t id, uint32_t face) const = 0;

		// Use this instead of constructor
		static Framebuffer* Create(uint32_t width, uint32_t height, bool isEnvironment = false);
	};

}