#include "gepch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace ge {

	OpenGLFramebuffer::OpenGLFramebuffer(uint32_t width, uint32_t height)
	{
		// Create the actual framebuffer
		glGenFramebuffers(1, &m_RendererID);
		//glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		// generate texture
		glGenTextures(1, &m_TexColorBuffer);
		glBindTexture(GL_TEXTURE_2D, m_TexColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glBindTexture(GL_TEXTURE_2D, 0);

		// Create Renderbuffer
		glGenRenderbuffers(1, &m_RenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
		// We want a depth and a stencil buffer
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Attach renderbuffer to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexColorBuffer, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);

		// Check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			GE_CORE_ERROR("Framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
	}

	void OpenGLFramebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::BindTexture() const
	{
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, m_TexColorBuffer);
	}

}