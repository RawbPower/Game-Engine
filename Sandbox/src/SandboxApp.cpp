// CLIENT

#include <GameEngine.h>

#include "imgui/imgui.h"

class ExampleLayer : public ge::Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_CameraRotation(0.0f)
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

		std::shared_ptr<ge::VertexBuffer> vertexBuffer;
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
		std::shared_ptr<ge::IndexBuffer> indexBuffer;
		indexBuffer.reset(ge::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Square vertex array test

		m_SquareVA.reset(ge::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,			// vertex coordinate on each row
			0.75f, -0.75f, 0.0f,
			0.75f, 0.75f, 0.0f,
			-0.75f, 0.75f, 0.0f
		};

		std::shared_ptr<ge::VertexBuffer> squareVB;
		squareVB.reset(ge::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
			{ ge::ShaderDataType::Float3, "a_Position" }
			});

		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<ge::IndexBuffer> squareIB;
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

			out vec3 v_Position;
			out vec4 v_Color;

			void main() {
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main() {
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		
		)";

		m_Shader.reset(new ge::Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main() {
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main() {
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		
		)";

		m_BlueShader.reset(new ge::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate() override
	{

		if (ge::Input::IsKeyPressed(GE_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed;
		else if (ge::Input::IsKeyPressed(GE_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed;

		if (ge::Input::IsKeyPressed(GE_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed;
		else if (ge::Input::IsKeyPressed(GE_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed;

		// For some reason the positive rotation is clockwise
		if (ge::Input::IsKeyPressed(GE_KEY_A))
			m_CameraRotation += m_CameraRotSpeed;

		if (ge::Input::IsKeyPressed(GE_KEY_D))
			m_CameraRotation -= m_CameraRotSpeed;

		ge::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		ge::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		ge::Renderer::BeginScene(m_Camera);

		ge::Renderer::Submit(m_BlueShader, m_SquareVA);
		ge::Renderer::Submit(m_Shader, m_VertexArray);

		ge::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(ge::Event& event) override
	{

	}

	bool OnKeyPressedEvent(ge::KeyPressedEvent& event) 
	{

	}

private:
	std::shared_ptr<ge::Shader> m_Shader;
	std::shared_ptr<ge::VertexArray> m_VertexArray;

	std::shared_ptr<ge::Shader> m_BlueShader;
	std::shared_ptr<ge::VertexArray> m_SquareVA;

	ge::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraMoveSpeed = 0.1f;
	float m_CameraRotSpeed = 2.0f;
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