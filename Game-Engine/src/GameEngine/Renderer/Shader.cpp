#include "gepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace ge {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& pixelSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		GE_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;

		case RendererAPI::API::OpenGL:	return new OpenGLShader(vertexSrc, pixelSrc);
		}

		GE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}