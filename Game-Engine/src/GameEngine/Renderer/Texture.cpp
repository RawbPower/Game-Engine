/*
	Texture

	Abstract class for texture inherited by classes for each Renderering API
*/

#include "gepch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace ge {

	Ref<Texture2D> Texture2D::Create(const std::string& path, bool gammaCorrection) 
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		GE_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;

			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLTexture2D>(path, gammaCorrection);
		}

		GE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


	Ref<Texture3D> Texture3D::Create(const std::string& path, const std::string& directory)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		GE_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;

		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLTexture3D>(path, directory);
		}

		GE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


	Ref<Cubemap> Cubemap::Create(const std::vector<std::string> faces)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		GE_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;

		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLCubemap>(faces);
		}

		GE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


	Ref<HDREnvironmentMap> HDREnvironmentMap::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		GE_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;

		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLHDREnvironmentMap>(path);
		}

		GE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}