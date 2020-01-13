// CLIENT

#include <GameEngine.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

class ExampleLayer : public ge::Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_OrthogonalCameraController(1280.0f / 720.0f, true), m_PerspectiveCameraController(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f), m_Scene(Scene::Scene3D)
	{
		if (m_Scene == Scene::Scene2D) {
			/* Vertex Array (required for core OpenGL profile) */
			m_VertexArray.reset(ge::VertexArray::Create());
			//m_VertexArray->Bind();

			/* Vertex Buffer */

			// 3D coordinates and there are 3 of them
			// OpenGl default clip space is -1 to 1 x, y, z
			float vertices[3 * 7] = {
				-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,				// vertex coordinate on each row
				0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
				0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
			};

			ge::Ref<ge::VertexBuffer> vertexBuffer;
			vertexBuffer.reset(ge::VertexBuffer::Create(vertices, sizeof(vertices)));

			ge::BufferLayout layout = {
				{ ge::ShaderDataType::Float3, "a_Position" },
				{ ge::ShaderDataType::Float4, "a_Color" },
			};

			vertexBuffer->SetLayout(layout);

			// Tell OpenGL what the layout of the buffer is
			m_VertexArray->AddVertexBuffer(vertexBuffer);


			/* Index Buffer (gives index to vertices, describes what order to draw vertices) */

			uint32_t indices[3] = { 0, 1, 2 };		// 3 points in the triangle
			ge::Ref<ge::IndexBuffer> indexBuffer;
			indexBuffer.reset(ge::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
			m_VertexArray->SetIndexBuffer(indexBuffer);

			// Square vertex array test

			m_SquareVA.reset(ge::VertexArray::Create());

			float squareVertices[5 * 4] = {
				-0.5f, -0.5f, 0.0f,	0.0f, 0.0f,		// 3 vertex coordinate on each row and 2 texture coords
				0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
				-0.5f, 0.5f, 0.0f, 0.0f, 1.0f
			};

			ge::Ref<ge::VertexBuffer> squareVB;
			squareVB.reset(ge::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

			squareVB->SetLayout({
				{ ge::ShaderDataType::Float3, "a_Position" },
				{ ge::ShaderDataType::Float2, "a_TexCoord" }
				});

			m_SquareVA->AddVertexBuffer(squareVB);

			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
			ge::Ref<ge::IndexBuffer> squareIB;
			squareIB.reset(ge::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
			m_SquareVA->SetIndexBuffer(squareIB);

			/* Shader (If we do nothing GPU drivers will make a default one) */

			// Shader source code
			// R prefix allows multiple line strings
			std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main() {
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		
			)";

			std::string pixelSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main() {
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		
			)";

			m_Shader = ge::Shader::Create("VertexPosColor", vertexSrc, pixelSrc);


			std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main() {
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		
			)";

			std::string flatColorShaderPixelSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main() {
				color = vec4(u_Color, 1.0);
			}
		
			)";

			m_FlatColorShader = ge::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderPixelSrc);

			auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

			m_Texture = ge::Texture2D::Create("assets/textures/container.jpg");
			m_BlendTexture = ge::Texture2D::Create("assets/textures/awesomeface.png");

			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture1", 0);		// 0 is the texure slot of m_Texture
		} 
		else if (m_Scene == Scene::Scene3D)
		{
			// Square vertex array test

			m_CubeVA.reset(ge::VertexArray::Create());

			float cubeVertices[] = {
				-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
				 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
				 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
				 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

				-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

				 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
				 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
				 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
			};

			// world space positions of our cubes
			m_CubePositions[0] = glm::vec3(0.0f, 0.0f, 0.0f);
			m_CubePositions[1] = glm::vec3(2.0f, 5.0f, -15.0f);
			m_CubePositions[2] = glm::vec3(-1.5f, -2.2f, -2.5f);
			m_CubePositions[3] = glm::vec3(-3.8f, -2.0f, -12.3f);
			m_CubePositions[4] = glm::vec3(2.4f, -0.4f, -3.5f);
			m_CubePositions[5] = glm::vec3(-1.7f, 3.0f, -7.5f);
			m_CubePositions[6] = glm::vec3(1.3f, -2.0f, -2.5f);
			m_CubePositions[7] = glm::vec3(1.5f, 2.0f, -2.5f);
			m_CubePositions[8] = glm::vec3(1.5f, 0.2f, -1.5f);
			m_CubePositions[9] = glm::vec3(-1.3f, 1.0f, -1.5f);

			ge::Ref<ge::VertexBuffer> cubeVB;
			cubeVB.reset(ge::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

			cubeVB->SetLayout({
				{ ge::ShaderDataType::Float3, "a_Position" },
				{ ge::ShaderDataType::Float2, "a_TexCoord" }
			});

			m_CubeVA->AddVertexBuffer(cubeVB);

			//uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
			//ge::Ref<ge::IndexBuffer> squareIB;
			//squareIB.reset(ge::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
			//m_SquareVA->SetIndexBuffer(squareIB);

			/* Shader (If we do nothing GPU drivers will make a default one) */

			auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

			m_Texture = ge::Texture2D::Create("assets/textures/container.jpg");

			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);		// 0 is the texure slot of m_Texture
		}
	}

	void OnUpdate(ge::DeltaTime dt) override
	{
		if (m_Scene == Scene::Scene2D) {
			// Update
			m_OrthogonalCameraController.OnUpdate(dt);

			// Render
			ge::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			ge::RenderCommand::Clear();

			ge::Renderer::BeginScene(m_OrthogonalCameraController.GetCamera());

			static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

			// Set up uniform
			std::dynamic_pointer_cast<ge::OpenGLShader>(m_FlatColorShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

			// Render grid of blue squares
			for (int x = -10; x < 11; x++) {
				for (int y = -10; y < 11; y++) {
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
					ge::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
				}
			}

			auto textureShader = m_ShaderLibrary.Get("Texture");

			m_Texture->Bind(0);
			ge::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

			m_BlendTexture->Bind(0);
			ge::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

			// Render triangle
			//ge::Renderer::Submit(m_Shader, m_VertexArray);

			ge::Renderer::EndScene();
		}
		else if (m_Scene == Scene::Scene3D)
		{
			// Update
			m_PerspectiveCameraController.OnUpdate(dt);

			// Render
			ge::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			ge::RenderCommand::Clear();

			ge::Renderer::BeginScene(m_PerspectiveCameraController.GetCamera());
			//ge::Renderer::BeginScene(m_CameraController.GetCamera());

			static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			//m_CubeRotation -= 1.0f * dt;
			//glm::mat4 transform = glm::rotate(glm::mat4(1.0f), m_CubeRotation, glm::vec3(0.5f, 1.0f, 0.0f));

			auto textureShader = m_ShaderLibrary.Get("Texture");

			m_Texture->Bind(0);
			//ge::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
			//ge::Renderer::Submit3D(textureShader, m_CubeVA, transform, 36);			// since the cube has no indexBuffer I will added the number of vertices manually. Will change in future

			for (unsigned int i = 0; i < 10; i++)
			{
				// calculate the model matrix for each object and pass it to shader before drawing
				glm::mat4 transform = glm::mat4(1.0f);
				transform = glm::translate(transform, m_CubePositions[i]);
				transform = glm::rotate(transform, glm::radians(m_CubeRotations[i]), glm::vec3(1.0f, 0.3f, 0.5f));
				ge::Renderer::Submit3D(textureShader, m_CubeVA, transform, 36);
			}

			ge::Renderer::EndScene();
		}
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(ge::Event& e) override
	{
		if (m_Scene == Scene::Scene2D)
			m_OrthogonalCameraController.OnEvent(e);
		else if (m_Scene == Scene::Scene3D)
			m_PerspectiveCameraController.OnEvent(e);
	}

	bool OnKeyPressedEvent(ge::KeyPressedEvent& event) 
	{

	}

private:
	ge::ShaderLibrary m_ShaderLibrary;

	ge::Ref<ge::Shader> m_Shader;
	ge::Ref<ge::VertexArray> m_VertexArray;

	ge::Ref<ge::Shader> m_FlatColorShader;
	ge::Ref<ge::VertexArray> m_SquareVA;

	ge::Ref<ge::Shader> m_CubeShader;
	ge::Ref<ge::VertexArray> m_CubeVA;

	ge::Ref<ge::Texture2D> m_Texture, m_BlendTexture;

	ge::OrthographicCameraController m_OrthogonalCameraController;
	ge::PerspectiveCameraController m_PerspectiveCameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };

	float m_CubeRotation = 0.0f;
	float m_CubeRotationSpeed = 50.0f;

	glm::vec3 m_CubePositions[10];
	float m_CubeRotations[10] = { 0.0f, 20.0f, 40.0f, 60.0f, 80.0f, 100.0f, 120.0f, 140.0f, 160.0f, 180.0f };

	float m_TotalTime = 0.0f;

	enum class Scene : uint32_t
	{
		Scene2D = 0, Scene3D = 1
	};

	Scene m_Scene;

};


class Sandbox : public ge::Application
{
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};

ge::Application* ge::CreateApplication() {

	return new Sandbox();
}