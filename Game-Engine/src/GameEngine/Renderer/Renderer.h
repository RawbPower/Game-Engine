/*
	Renderer

	Class for the Renderer of the application
*/

#pragma once

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "Shader.h"

namespace ge {

	class Renderer {
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);		// We take in a reference to the camera because OrthographicCamera is a large class
		static void BeginScene(PerspectiveCamera& camera);
		static void EndScene();

		static void SetProjection(const std::shared_ptr<Shader>& shader, const glm::mat4& transform);

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, unsigned int vertices, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitFramebuffer(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, unsigned int vertices);
		static void SubmitSkybox(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, unsigned int vertices);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData 
		{
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};
}
