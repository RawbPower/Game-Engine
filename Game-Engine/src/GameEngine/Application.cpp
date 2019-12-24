#include "gepch.h"
#include "Application.h"

#include "GameEngine/Log.h"

#include <glad/glad.h>

#include "Input.h"

namespace ge {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		// Making application a singleton
		GE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		/* Vertex Array (required for core OpenGL profile) */
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		/* Vertex Buffer */
		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		// 3D coordinates and there are 3 of them
		// OpenGl default clip space is -1 to 1 x, y, z
		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,				// vertex coordinate on each row
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};

		// Upload date to GPU (it is in CPU)
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Tell OpenGL what the layout of the buffer is
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		/* Index Buffer (gives index to vertices, describes what order to draw vertices) */
		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };		// 3 points in the triangle
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		/* Shader (If we do nothing GPU drivers will make a default one) */

		// Shader source code
		// R prefix allows multiple line strings
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main() {
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main() {
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		
		)";
		
		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
	}


	Application::~Application()
	{
	}

	// Essentailly a wrapper
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e) {

		// Dispatch events
		EventDispatcher dispatcher(e);
		// Tell disptacher "If you see a window close event that dispatch it to this function"
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		//GE_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run() {
		while (m_Running) {
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			// Render ImGui
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
