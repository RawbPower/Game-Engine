// CLIENT

#include <GameEngine.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

class ExampleLayer : public ge::Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_OrthographicCameraController(1280.0f / 720.0f, true), m_PerspectiveCameraController(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f), m_Model(""), m_Scene(Scene::Scene3D)
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

			// Create textures
			m_Texture = ge::Texture2D::Create("assets/textures/Checkerboard.png");
			m_BlendTexture = ge::Texture2D::Create("assets/textures/ChernoLogo.png");

			// Bind shader and upload texture uniform
			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);		// 0 is the texure slot of m_Texture
		} 
		else if (m_Scene == Scene::Scene3D)
		{
			// Enable z-buffer for 3D rendering only
			ge::RenderCommand::EnableZBuffer();

			// Create VAOs for all the objects
			m_CubeVA.reset(ge::VertexArray::Create());
			m_SkyboxVA.reset(ge::VertexArray::Create());

			// Set up vertices for the objects
			float cubeVertices[] = {
				// positions          // normals
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
				 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
				 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
				 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
				 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
			};

			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			// Set buffer layout for cube
			ge::Ref<ge::VertexBuffer> cubeVB;
			cubeVB.reset(ge::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

			cubeVB->SetLayout({
				{ ge::ShaderDataType::Float3, "a_Position" },
				{ ge::ShaderDataType::Float3, "a_Normal" }
				});

			m_CubeVA->AddVertexBuffer(cubeVB);

			// Set buffer layout for skybox
			ge::Ref<ge::VertexBuffer> skyboxVB;
			skyboxVB.reset(ge::VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices)));

			skyboxVB->SetLayout({
				{ ge::ShaderDataType::Float3, "a_Position" }
			});

			m_SkyboxVA->AddVertexBuffer(skyboxVB);

			// Create model with relative path
			m_Model = ge::Model("assets/nanosuit/nanosuit.obj");


			// Shaders
			auto shader = m_ShaderLibrary.Load("assets/shaders/Reflection.glsl");
			auto skyboxShader = m_ShaderLibrary.Load("assets/shaders/Skybox.glsl");


			m_CubeTexture = ge::Texture2D::Create("assets/textures/container.jpg");

			std::dynamic_pointer_cast<ge::OpenGLShader>(shader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(shader)->UploadUniformInt("u_Skybox", 0);		// 0 is the texure slot of m_Texture

			std::vector<std::string> faces
			{
				"assets/textures/skybox/right.jpg",
				"assets/textures/skybox/left.jpg",
				"assets/textures/skybox/top.jpg",
				"assets/textures/skybox/bottom.jpg",
				"assets/textures/skybox/front.jpg",
				"assets/textures/skybox/back.jpg"
			};

			m_SkyboxMap = ge::Cubemap::Create(faces);

			std::dynamic_pointer_cast<ge::OpenGLShader>(skyboxShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(skyboxShader)->UploadUniformInt("u_Skybox", 0);
		}
	}

	// Function called every frame
	void OnUpdate(ge::DeltaTime dt) override
	{
		if (m_Scene == Scene::Scene2D) {
			// Update
			m_OrthographicCameraController.OnUpdate(dt);

			// Render
			ge::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			ge::RenderCommand::Clear();

			ge::Renderer::BeginScene(m_OrthographicCameraController.GetCamera());

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

			// Get shader from library
			auto textureShader = m_ShaderLibrary.Get("Texture");

			// Bind texures and dray shapes
			m_Texture->Bind(0);
			ge::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

			m_BlendTexture->Bind(0);
			ge::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

			ge::Renderer::EndScene();
		}
		else if (m_Scene == Scene::Scene3D)
		{
			// Update Camera
			m_PerspectiveCameraController.OnUpdate(dt);

			// Bind framebuffer
			ge::RenderCommand::EnableZBuffer();

			// Rendering
			// Clear previous frame
			ge::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			ge::RenderCommand::Clear();

			// Begin the current scene
			ge::Renderer::BeginScene(m_PerspectiveCameraController.GetCamera());

			//----Object being lit rendering---//
			auto textureShader = m_ShaderLibrary.Get("Reflection");

			// Set up uniforms
			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->UploadUniformFloat3("u_ViewPosition", m_PerspectiveCameraController.GetCameraPosition());

			// Cubes	
			/*m_CubeTexture->Bind(0);
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f));
			ge::Renderer::Submit(textureShader, m_CubeVA, 36, transform);*/

			// Position the model
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(0.0f, -1.75f, 0.0f));
			transform = glm::scale(transform, glm::vec3(0.2f, 0.2f, 0.2f));

			// Setup up the prejection matrix for the camera
			ge::Renderer::SetProjection(textureShader, transform);

			// Draw model to screen
			m_Model.Draw(textureShader);

			// Skybox
			ge::RenderCommand::DepthFunc("LEQUAL");	// change depth function so depth test passes when values are equal to depth buffer's content

			auto skyboxShader = m_ShaderLibrary.Get("Skybox");

			// Set up uniforms
			std::dynamic_pointer_cast<ge::OpenGLShader>(skyboxShader)->Bind();

			m_SkyboxMap->Bind(0);
			ge::Renderer::SubmitSkybox(skyboxShader, m_SkyboxVA, 36);

			ge::RenderCommand::DepthFunc("LESS"); // set depth function back to default

			ge::Renderer::EndScene();
		}
	}

	// Adding GUI rendering 
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(ge::Event& e) override
	{
		if (m_Scene == Scene::Scene2D)
			m_OrthographicCameraController.OnEvent(e);
		else if (m_Scene == Scene::Scene3D)
			m_PerspectiveCameraController.OnEvent(e);
	}

	bool OnKeyPressedEvent(ge::KeyPressedEvent& event) 
	{

	}

private:

	// General Variables
	ge::ShaderLibrary m_ShaderLibrary;					// Library for shader files

	ge::Ref<ge::Texture2D> m_Texture, m_BlendTexture, m_SpecularMap;	// Texture files

	float m_TotalTime = 0.0f;							// Total time passed in application life time (mod 2pi)

	// 2D Scene Variables
	ge::OrthographicCameraController m_OrthographicCameraController;	// Orthographic Camera Controller

	ge::Ref<ge::Shader> m_Shader;					// Basic shader test
	ge::Ref<ge::VertexArray> m_VertexArray;			// Basic vertex array for traingle

	ge::Ref<ge::Shader> m_FlatColorShader;			// Flat color shader
	ge::Ref<ge::VertexArray> m_SquareVA;			// Vertex array for square

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };	// Square color

	// 3D Scene Varaibles
	ge::PerspectiveCameraController m_PerspectiveCameraController;	// Perspective Camera Controller

	ge::Ref<ge::Shader> m_CubeShader;
	ge::Ref<ge::VertexArray> m_CubeVA;

	ge::Ref<ge::Shader> m_SkyboxShader;
	ge::Ref<ge::VertexArray> m_SkyboxVA;

	ge::Ref<ge::Cubemap> m_SkyboxMap;

	ge::Ref<ge::Texture2D> m_CubeTexture;

	ge::Model m_Model;								// Model to be rendered

	// Enumerator for switching between 2D and 3D rendering
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