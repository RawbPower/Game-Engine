#pragma once

#include "GameEngine/Renderer/RendererAPI.h"

namespace ge {

	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void Init() override;
		virtual void EnableZBuffer() override;
		virtual void WireFrame() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
		virtual void DrawIndexed(const std::vector<unsigned int> indices) override;
		virtual void DrawVertices(int vertices) override;
	};
}
