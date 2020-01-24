#include "gepch.h"
#include "Framebuffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace ge {

	Framebuffer* Framebuffer::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		GE_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;

		case RendererAPI::API::OpenGL:		return new OpenGLFramebuffer(width, height);
		}

		GE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}