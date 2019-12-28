// CLIENT

#include <GameEngine.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

class ExampleLayer : public ge::Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_CameraRotation(0.0f), m_SquarePosition(0.0f)
	{
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

		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,			// vertex coordinate on each row
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
		};

		ge::Ref<ge::VertexBuffer> squareVB;
		squareVB.reset(ge::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
			{ ge::ShaderDataType::Float3, "a_Position" }
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

		m_Shader.reset(ge::Shader::Create(vertexSrc, pixelSrc));

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

		m_FlatColorShader.reset(ge::Shader::Create(flatColorShaderVertexSrc, flatColorShaderPixelSrc));
	}

	void OnUpdate(ge::DeltaTime dt) override
	{

		GE_TRACE("Delta time: {0}s ({1}ms)", dt.GetSeconds(), dt.GetMilliseconds());

		// Camera Movement
		if (ge::Input::IsKeyPressed(GE_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * dt;
		else if (ge::Input::IsKeyPressed(GE_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * dt;

		if (ge::Input::IsKeyPressed(GE_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * dt;
		else if (ge::Input::IsKeyPressed(GE_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * dt;

		// Camera Rotation
		// For some reason the positive rotation is clockwise
		if (ge::Input::IsKeyPressed(GE_KEY_A))
			m_CameraRotation += m_CameraRotSpeed * dt;

		if (ge::Input::IsKeyPressed(GE_KEY_D))
			m_CameraRotation -= m_CameraRotSpeed * dt;

		// Game Object transform movement
		if (ge::Input::IsKeyPressed(GE_KEY_J))
			m_SquarePosition.x -= m_SquareMoveSpeed * dt;
		else if (ge::Input::IsKeyPressed(GE_KEY_L))
			m_SquarePosition.x += m_SquareMoveSpeed * dt;

		if (ge::Input::IsKeyPressed(GE_KEY_K))
			m_SquarePosition.y -= m_SquareMoveSpeed * dt;
		else if (ge::Input::IsKeyPressed(GE_KEY_I))
			m_SquarePosition.y += m_SquareMoveSpeed * dt;

		ge::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		ge::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		ge::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<ge::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<ge::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		// Render grid of blue squares
		for (int x = -10; x < 11; x++) {
			for (int y = -10; y < 11; y++) {
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_SquarePosition + pos) * scale;
				ge::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		// Render triangle
		ge::Renderer::Submit(m_Shader, m_VertexArray);

		ge::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(ge::Event& event) override
	{

	}

	bool OnKeyPressedEvent(ge::KeyPressedEvent& event) 
	{

	}

private:
	ge::Ref<ge::Shader> m_Shader;
	ge::Ref<ge::VertexArray> m_VertexArray;

	ge::Ref<ge::Shader> m_FlatColorShader;
	ge::Ref<ge::VertexArray> m_SquareVA;

	ge::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotSpeed = 180.0f;

	glm::vec3  m_SquarePosition;
	float m_SquareMoveSpeed = 1.0f;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
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