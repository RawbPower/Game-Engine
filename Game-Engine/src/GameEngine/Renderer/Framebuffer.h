#pragma once

namespace ge {

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void BindTexture() const = 0;

		// Use this instead of constructor
		static Framebuffer* Create(uint32_t width, uint32_t height);
	};

}