#include "gepch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace ge {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		s_SceneData->ViewMatrix = camera.GetViewMatrix();
		s_SceneData->ProjectionMatrix = camera.GetProjectionMatrix();
	}

	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		s_SceneData->ViewMatrix = camera.GetViewMatrix();
		s_SceneData->ProjectionMatrix = camera.GetProjectionMatrix();
	}

	void Renderer::EndScene()
	{
		// Does nothing for now
	}

	void Renderer::SetProjection(const std::shared_ptr<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f))
	{
		//shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
	}

	// The vertex array will be submitted into a RenderCommand queue to be evaluated later and get rendered
	// For now it is much simpler
	// Shader needs to be a parameter in submit because it can change for different objects in the scene
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, unsigned int vertices, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawVertices(vertices);
	}

	void Renderer::SubmitFramebuffer(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, unsigned int vertices)
	{
		vertexArray->Bind();
		RenderCommand::DrawVertices(vertices);
	}

	void Renderer::SubmitSkybox(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, unsigned int vertices)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_View", glm::mat4(glm::mat3(s_SceneData->ViewMatrix)));  // remove translation from the view matrix
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Projection", s_SceneData->ProjectionMatrix);

		vertexArray->Bind();
		RenderCommand::DrawVertices(vertices);
	}

}