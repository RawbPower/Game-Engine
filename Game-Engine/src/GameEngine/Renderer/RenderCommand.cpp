/*
	Render Command

	Class for the commands for the renderer. Needs to be put into a render queue.
*/

#include "gepch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace ge {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}