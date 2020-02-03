/*
	Buffer

	Abstract class for vetex and index buffers inherited by classes for each Renderering API
*/

#include "gepch.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace ge {

	VertexBuffer* VertexBuffer::Create(void* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI()) 
		{
			case RendererAPI::API::None:		GE_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;

			case RendererAPI::API::OpenGL:	return new OpenGLVertexBuffer(vertices, size);
		}

		GE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(void* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		GE_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;

			case RendererAPI::API::OpenGL:	return new OpenGLIndexBuffer(indices, count);
		}

		GE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}