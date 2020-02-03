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
		OpenGLFramebuffer(uint32_t width, uint32_t height);
		virtual ~OpenGLFramebuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual void BindTexture() const;

	private:
		uint32_t m_RendererID;
		uint32_t m_TexColorBuffer;
		uint32_t m_RenderBuffer;
	};

}
