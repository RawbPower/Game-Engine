// CLIENT

#include <GameEngine.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

class ExampleLayer : public ge::Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_OrthographicCameraController(1280.0f / 720.0f, true), m_PerspectiveCameraController(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f), m_Scene(Scene::Scene3D)
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

			m_Texture = ge::Texture2D::Create("assets/textures/Checkerboard.png");
			m_BlendTexture = ge::Texture2D::Create("assets/textures/ChernoLogo.png");

			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);		// 0 is the texure slot of m_Texture
		} 
		else if (m_Scene == Scene::Scene3D)
		{
			// Enable z-buffer for 3D rendering only
			ge::RenderCommand::EnableZBuffer();

			// Instatiate vertex arrays for object and light source
			m_CubeVA.reset(ge::VertexArray::Create());
			m_LightCubeVA.reset(ge::VertexArray::Create());

			// Vertices for cube
			float cubeVertices[] = {
				// positions          // normals           // texture coords
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
				 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
				 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
				 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
				 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
				 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
				 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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

			m_PointLightPositions[0] = glm::vec3(0.7f, 0.2f, 2.0f);
			m_PointLightPositions[1] = glm::vec3(2.3f, -3.3f, -4.0f);
			m_PointLightPositions[2] = glm::vec3(-4.0f, 2.0f, -12.0f);
			m_PointLightPositions[3] = glm::vec3(0.0f, 0.0f, -3.0f);

			// Create a vertex buffer for the object
			ge::Ref<ge::VertexBuffer> cubeVB;
			cubeVB.reset(ge::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

			cubeVB->SetLayout({
				{ ge::ShaderDataType::Float3, "a_Position" },
				{ ge::ShaderDataType::Float3, "a_Normal" },
				{ ge::ShaderDataType::Float2, "a_TexCoords" }
			});

			m_CubeVA->AddVertexBuffer(cubeVB);

			// Even though the light is also a cube we want a different vertex buffer because it will change vertices in the future
			ge::Ref<ge::VertexBuffer> lightCubeVB;
			lightCubeVB.reset(ge::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

			lightCubeVB->SetLayout({
				{ ge::ShaderDataType::Float3, "a_Position" },
				{ ge::ShaderDataType::Float3, "a_Normal" },
				{ ge::ShaderDataType::Float2, "a_TexCoords" }
			});

			m_LightCubeVA->AddVertexBuffer(lightCubeVB);


			/* Shaders */
			auto lightingShader = m_ShaderLibrary.Load("assets/shaders/MultipleLights.glsl");

			m_Texture = ge::Texture2D::Create("assets/textures/container2.png");
			m_SpecularMap = ge::Texture2D::Create("assets/textures/container2_specular.png");

			std::dynamic_pointer_cast<ge::OpenGLShader>(lightingShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(lightingShader)->UploadUniformInt("material.diffuse", 0);		// 0 is the texure slot of m_Texture
			std::dynamic_pointer_cast<ge::OpenGLShader>(lightingShader)->UploadUniformInt("material.specular", 1);

			auto lampShader = m_ShaderLibrary.Load("assets/shaders/Lamp.glsl");
		}
	}

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
			// Update Camera
			m_PerspectiveCameraController.OnUpdate(dt);

			// Rendering
			// Clear previous frame
			ge::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			ge::RenderCommand::Clear();

			// Begin the current scene
			ge::Renderer::BeginScene(m_PerspectiveCameraController.GetCamera());

			//----Object being lit rendering---//
			auto lightingShader = m_ShaderLibrary.Get("MultipleLights");

			// Add textures
			m_Texture->Bind(0);
			m_SpecularMap->Bind(1);

			// Set up uniforms
			std::dynamic_pointer_cast<ge::OpenGLShader>(lightingShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(lightingShader)->UploadUniformFloat3("u_ViewPosition", m_PerspectiveCameraController.GetCameraPosition());
			std::dynamic_pointer_cast<ge::OpenGLShader>(lightingShader)->UploadUniformFloat("material.shininess", 32.0f);

			// Light Uniforms
			// directional light
			ge::DirLight::UploadUniforms(lightingShader, "dirLight", { -0.2f, -1.0f, -0.3f },								// shader, name, direction
											{ 0.05f, 0.05f, 0.05f }, { 0.4f, 0.4f, 0.4f }, { 0.5f, 0.5f, 0.5f });			// ambient, diffuse, specular

			// point light 1
			ge::PointLight::UploadUniforms(lightingShader, "pointLights[0]", m_PointLightPositions[0],				// shader, name, position
											{ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f },	// ambient, diffuse, specular
											1.0f, 0.09, 0.032);														// constant, linear, quadratic	

			// point light 2
			ge::PointLight::UploadUniforms(lightingShader, "pointLights[1]", m_PointLightPositions[1],				// shader, name, position
											{ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f },	// ambient, diffuse, specular
											1.0f, 0.09, 0.032);														// constant, linear, quadratic

			// point light 3
			ge::PointLight::UploadUniforms(lightingShader, "pointLights[2]", m_PointLightPositions[2],				// shader, name, position
											{ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f },	// ambient, diffuse, specular
											1.0f, 0.09, 0.032);														// constant, linear, quadratic

			// point light 4
			ge::PointLight::UploadUniforms(lightingShader, "pointLights[3]", m_PointLightPositions[3],				// shader, name, position
											{ 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f },	// ambient, diffuse, specular
											1.0f, 0.09, 0.032);														// constant, linear, quadratic

			// spotLight
			ge::SpotLight::UploadUniforms(lightingShader, "spotLight",												// shader, name
											m_PerspectiveCameraController.GetCameraPosition(),						// position
											m_PerspectiveCameraController.GetCameraFront(),							// direction
											{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },		// ambient, linear, specular
											1.0f, 0.09, 0.032,														// constant, linear, quadratic
											12.5f, 15.0f);															// cutOff, outerCutOff

			// calculate the model matrix for ethr object and pass it to shader before drawing
			for (unsigned int i = 0; i < 10; i++)
			{
				// calculate the model matrix for each object and pass it to shader before drawing
				glm::mat4 transform = glm::mat4(1.0f);
				transform = glm::translate(transform, m_CubePositions[i]);
				//m_CubeRotations[i] -= m_CubeRotationSpeed * dt;
				transform = glm::rotate(transform, glm::radians(m_CubeRotations[i]), glm::vec3(1.0f, 0.3f, 0.5f));
				ge::Renderer::Submit3D(lightingShader, m_CubeVA, transform, 36);
			}

			//----Lamp rendering---//
			auto lampShader = m_ShaderLibrary.Get("Lamp");
			std::dynamic_pointer_cast<ge::OpenGLShader>(lampShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(lampShader)->UploadUniformFloat3("u_LightColor", { 1.0f, 1.0f, 1.0f });

			for (unsigned int i = 0; i < 4; i++)
			{
				glm::mat4 lampTransform = glm::mat4(1.0f);
				lampTransform = glm::translate(lampTransform, m_PointLightPositions[i]);
				lampTransform = glm::scale(lampTransform, glm::vec3(0.2f));
				ge::Renderer::Submit3D(lampShader, m_LightCubeVA, lampTransform, 36);
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

	ge::Ref<ge::Shader> m_CubeShader;			// Shader for cube
	ge::Ref<ge::VertexArray> m_CubeVA;			// Vertex array for cube

	ge::Ref<ge::Shader> m_LightCubeShader;		// Shader for light source
	ge::Ref<ge::VertexArray> m_LightCubeVA;		// Vertex array for light source

	glm::vec3 m_ObjectColor = { 1.0f, 0.5f, 0.31f };	// Base colour of object being lit (in white light)
	glm::vec3 m_LightColor = { 1.0f, 1.0f, 1.0f };		// Light colour

	float m_CubeRotation = 0.0f;						// Initial roation of cube (Currently not used)
	float m_CubeRotationSpeed = 50.0f;					// Rotation speed of cube (Currently not used)

	glm::vec3 m_CubePosition = glm::vec3(1.0f);					// Position of cube
	glm::vec3 m_LightPosition = glm::vec3(1.2f, 1.0f, 2.0f);	// Position of light source

	// Used when multiple are on screen
	glm::vec3 m_CubePositions[10];
	glm::vec3 m_PointLightPositions[4];
	float m_CubeRotations[10] = { 0.0f, 20.0f, 40.0f, 60.0f, 80.0f, 100.0f, 120.0f, 140.0f, 160.0f, 180.0f };

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