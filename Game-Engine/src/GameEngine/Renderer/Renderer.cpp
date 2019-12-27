#include "gepch.h"
#include "Renderer.h"

namespace ge {

	void Renderer::BeginScene()
	{
		// Does nothing for now
	}

	void Renderer::EndScene()
	{
		// Does nothing for now
	}

	// The vertex array will be submitted into a RenderCommand queue to be evaluated later and get rendered
	// For now it is much simpler
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}