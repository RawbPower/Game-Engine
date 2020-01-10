#pragma once

#include "RendererAPI.h"

namespace ge {

	// Works basically as a wrapper for the RendererAPI
	class RenderCommand 
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

		inline static void DrawIndexed3D(const std::shared_ptr<VertexArray>& vertexArray, int vertices)
		{
			s_RendererAPI->DrawIndexed3D(vertexArray, vertices);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}
