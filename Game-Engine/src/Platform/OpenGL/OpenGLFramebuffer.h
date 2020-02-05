/*
	OpenGL Framebuffer

	Framebuffer class in OpenGL
*/

#pragma once

#include "GameEngine/Renderer/Framebuffer.h"

namespace ge {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(uint32_t width, uint32_t height, bool isEnvironment);
		virtual ~OpenGLFramebuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual void Rescale(const uint32_t width, const uint32_t height) const;
		virtual void BindTexture() const;
		virtual void AttachCubemapTexture(uint32_t id, uint32_t face) const;

	private:
		uint32_t m_RendererID;
		uint32_t m_TexColorBuffer;
		uint32_t m_RenderBuffer;
	};

}
