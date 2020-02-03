/*
	Renderer API

	Abstract class for the renderer API inherited by classes for each Renderering API
*/

#include "gepch.h"
#include "RendererAPI.h"

namespace ge {
	
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
}