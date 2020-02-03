// Client applications using the game engine library

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
				{ ge::ShaderDataType::Float2, "a_TexCoords" }
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
			m_PbrVA.reset(ge::VertexArray::Create());

			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			std::vector<uint32_t> indices;

			const unsigned int X_SEGMENTS = 64;
			const unsigned int Y_SEGMENTS = 64;
			const float PI = 3.14159265359;
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					positions.push_back(glm::vec3(xPos, yPos, zPos));
					uv.push_back(glm::vec2(xSegment, ySegment));
					normals.push_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = false;
			for (int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (int x = 0; x <= X_SEGMENTS; ++x)
					{
						indices.push_back(y * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (int x = X_SEGMENTS; x >= 0; --x)
					{
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}

			m_IndexCount = indices.size();

			std::vector<float> data;
			for (int i = 0; i < positions.size(); ++i)
			{
				data.push_back(positions[i].x);
				data.push_back(positions[i].y);
				data.push_back(positions[i].z);
				if (uv.size() > 0)
				{
					data.push_back(uv[i].x);
					data.push_back(uv[i].y);
				}
				if (normals.size() > 0)
				{
					data.push_back(normals[i].x);
					data.push_back(normals[i].y);
					data.push_back(normals[i].z);
				}
			}

			// Set buffer layout for cube
			ge::Ref<ge::VertexBuffer> pbrVB;
			pbrVB.reset(ge::VertexBuffer::Create(&data[0], data.size() * sizeof(float)));

			pbrVB->SetLayout({
				{ ge::ShaderDataType::Float3, "a_Position" },
				{ ge::ShaderDataType::Float2, "a_TexCoords" },
				{ ge::ShaderDataType::Float3, "a_Normal" }
				});

			m_PbrVA->AddVertexBuffer(pbrVB);	

			ge::Ref<ge::IndexBuffer> pbrIB;
			pbrIB.reset(ge::IndexBuffer::Create(&indices[0], indices.size()));
			m_PbrVA->SetIndexBuffer(pbrIB);

			// Shaders
			auto pbrShader = m_ShaderLibrary.Load("assets/shaders/PBR1.glsl");

			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->Bind();

			//m_Albedo = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/albedo.png");
			//m_Normal = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/normal.png");
			//m_Metallic = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/metallic.png");
			//m_Roughness = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/roughness.png");
			//m_Ao = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/ao.png");

			//std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_AlbedoMap", 0);
			//std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_NormalMap", 1);
			//std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_MetallicMap", 2);
			//std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_RoughnessMap", 3);
			//std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_AoMap", 4);

			// Fix this on update
			//m_Framebuffer.reset(ge::Framebuffer::Create(1280, 720));

			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformFloat3("u_Albedo", { 0.5f, 0.0f, 0.0f });
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformFloat("u_Ao", 1.0f);

			// lighting info
			// -------------
			// positions
			m_LightPositions.push_back(glm::vec3(-10.0f, 10.0f, 10.0f)); // back light
			m_LightPositions.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
			m_LightPositions.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
			m_LightPositions.push_back(glm::vec3(10.0f, -10.0f, 10.0f));
			// colors
			m_LightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
			m_LightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
			m_LightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
			m_LightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));

			//m_LightPositions.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
			//m_LightColors.push_back(glm::vec3(150.0f, 150.0f, 150.0f));

			//ge::RenderCommand::WireFrame();
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

			// Rendering
			// Clear previous frame
			ge::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			ge::RenderCommand::Clear();

			// Begin the current scene
			ge::Renderer::BeginScene(m_PerspectiveCameraController.GetCamera());

			auto pbrShader = m_ShaderLibrary.Get("PBR1");

			// Set up uniforms
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformFloat3("u_ViewPosition", m_PerspectiveCameraController.GetCameraPosition());

			// Bind texures
			//m_Albedo->Bind(0);
			//m_Normal->Bind(1);
			//m_Metallic->Bind(2);
			//m_Roughness->Bind(3);
			//m_Ao->Bind(4);


			// render rows*columns number of spheres with varying metallic/roughness values scaled by rows/columns respectively
			glm::mat4 transform = glm::mat4(1.0f);
			for (int row = 0; row < m_Rows; ++row)
			{
				std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformFloat("u_Metallic", (float)row / (float)m_Rows);
				for (int col = 0; col < m_Columns; ++col)
				{
					// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look
					// a bit off on direct lighting
					std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformFloat("u_Roughness", glm::clamp((float)col / (float)m_Columns, 0.05f, 1.0f));

					transform = glm::mat4(1.0f);
					transform = glm::translate(transform, glm::vec3((col - (m_Columns / 2)) * m_Spacing, (row - (m_Rows / 2)) * m_Spacing, 0.0f));
					ge::Renderer::Submit(pbrShader, m_PbrVA, transform);
				}
			}

			m_TotalTime += dt;
			// render light source (simply re-render sphere at light positions)
			// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
			// keeps the codeprint small.
			for (unsigned int i = 0; i < m_LightPositions.size(); ++i)
			{
				glm::vec3 newPos = m_LightPositions[i] + glm::vec3(sin(m_TotalTime * 5.0) * 5.0, 0.0, 0.0);
				newPos = m_LightPositions[i];
				std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformFloat3("u_LightPositions[" + std::to_string(i) + "]", newPos);
				std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformFloat3("u_LightColors[" + std::to_string(i) + "]", m_LightColors[i]);

				transform = glm::mat4(1.0f);
				transform = glm::translate(transform, newPos);
				transform = glm::scale(transform, glm::vec3(0.5f));
				ge::Renderer::Submit(pbrShader, m_PbrVA, transform);
			}

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

	ge::Ref<ge::VertexArray> m_PbrVA;

	ge::Ref<ge::Texture2D> m_Albedo, m_Normal, m_Metallic, m_Roughness, m_Ao;

	std::vector<glm::vec3> m_LightPositions;
	std::vector<glm::vec3> m_LightColors;

	ge::Ref<ge::Framebuffer> m_Framebuffer;

	ge::Model m_Model;								// Model to be rendered

	unsigned int m_IndexCount;
	int m_Rows = 7;
	int m_Columns = 7;
	float m_Spacing = 2.5;

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