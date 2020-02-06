/*
	OpenGL Renderer API

	Class for the functions use to set up the Renderer API
*/

#include "gepch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace ge {

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::EnableZBuffer()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::DisableZBuffer()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::DepthFunc(const std::string setting) 
	{
		if (setting == "EQUAL")
			glDepthFunc(GL_EQUAL);
		else if (setting == "LEQUAL")
			glDepthFunc(GL_LEQUAL);
		else if (setting == "GEQUAL")
			glDepthFunc(GL_GEQUAL);
		else if (setting == "LESS")
			glDepthFunc(GL_LESS);
		else if (setting == "GREATER")
			glDepthFunc(GL_GREATER);
		else
			GE_CORE_ERROR("Invalid depth function value: " + setting);
	}		

	void OpenGLRendererAPI::EnableSeamlessCubemap()
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void OpenGLRendererAPI::WireFrame()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
	
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear() 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLE_STRIP, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::vector<unsigned int> indices)
	{
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void OpenGLRendererAPI::DrawVertices(int vertices)
	{
		glDrawArrays(GL_TRIANGLES, 0, vertices);
	}

	void OpenGLRendererAPI::DrawVerticesStrip(int vertices)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices);
	}
}