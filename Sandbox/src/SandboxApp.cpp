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
		// Code for 3D Scene
		if (m_Scene == Scene::Scene3D)
		{
			// Enable z-buffer for 3D rendering only
			ge::RenderCommand::EnableZBuffer();
			ge::RenderCommand::DepthFunc("LEQUAL");
			// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
			ge::RenderCommand::EnableSeamlessCubemap();

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
			auto pbrShader = m_ShaderLibrary.Load("assets/shaders/PBR5.glsl");
			auto equirectangularToCubemapShader = m_ShaderLibrary.Load("assets/shaders/EquirectangularToCubemap.glsl");
			auto irradianceShader = m_ShaderLibrary.Load("assets/shaders/IBLIrradiance.glsl");
			auto prefilterShader = m_ShaderLibrary.Load("assets/shaders/Prefilter.glsl");
			auto brdfShader = m_ShaderLibrary.Load("assets/shaders/BRDF.glsl");
			auto backgroundShader = m_ShaderLibrary.Load("assets/shaders/Background.glsl");

			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_IrradianceMap", 0);
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_PrefilterMap", 1);
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_BrdfLUT", 2);
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_AlbedoMap", 3);
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_NormalMap", 4);
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_MetallicMap", 5);
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_RoughnessMap", 6);
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformInt("u_AoMap", 7);

			std::dynamic_pointer_cast<ge::OpenGLShader>(backgroundShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(backgroundShader)->UploadUniformInt("u_EnvironmentMap", 0);

			// Add textures
			// rusted iron
			m_IronAlbedo = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/albedo.png");
			m_IronNormal = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/normal.png");
			m_IronMetallic = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/metallic.png");
			m_IronRoughness = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/roughness.png");
			m_IronAo = ge::Texture2D::Create("assets/textures/pbr/rusted_iron/ao.png");

			// gold
			m_GoldAlbedo = ge::Texture2D::Create("assets/textures/pbr/gold/albedo.png");
			m_GoldNormal = ge::Texture2D::Create("assets/textures/pbr/gold/normal.png");
			m_GoldMetallic = ge::Texture2D::Create("assets/textures/pbr/gold/metallic.png");
			m_GoldRoughness = ge::Texture2D::Create("assets/textures/pbr/gold/roughness.png");
			m_GoldAo = ge::Texture2D::Create("assets/textures/pbr/gold/ao.png");

			// grass
			m_GrassAlbedo = ge::Texture2D::Create("assets/textures/pbr/grass/albedo.png");
			m_GrassNormal = ge::Texture2D::Create("assets/textures/pbr/grass/normal.png");
			m_GrassMetallic = ge::Texture2D::Create("assets/textures/pbr/grass/metallic.png");
			m_GrassRoughness = ge::Texture2D::Create("assets/textures/pbr/grass/roughness.png");
			m_GrassAo = ge::Texture2D::Create("assets/textures/pbr/grass/ao.png");

			// plastic
			m_PlasticAlbedo = ge::Texture2D::Create("assets/textures/pbr/plastic/albedo.png");
			m_PlasticNormal = ge::Texture2D::Create("assets/textures/pbr/plastic/normal.png");
			m_PlasticMetallic = ge::Texture2D::Create("assets/textures/pbr/plastic/metallic.png");
			m_PlasticRoughness = ge::Texture2D::Create("assets/textures/pbr/plastic/roughness.png");
			m_PlasticAo = ge::Texture2D::Create("assets/textures/pbr/plastic/ao.png");

			// wall
			m_WallAlbedo = ge::Texture2D::Create("assets/textures/pbr/wall/albedo.png");
			m_WallNormal = ge::Texture2D::Create("assets/textures/pbr/wall/normal.png");
			m_WallMetallic = ge::Texture2D::Create("assets/textures/pbr/wall/metallic.png");
			m_WallRoughness = ge::Texture2D::Create("assets/textures/pbr/wall/roughness.png");
			m_WallAo = ge::Texture2D::Create("assets/textures/pbr/wall/ao.png");

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

			// pbr: setup framebuffer (Framebuffer Environment)
			// ----------------------
			m_Framebuffer.reset(ge::Framebuffer::Create(512, 512, true));

			// pbr: load the HDR environment map (New texture type)
			// ---------------------------------

			m_HDREnvironmentMap = ge::HDREnvironmentMap::Create("assets/textures/hdr/newport_loft.hdr");

			// pbr: setup cubemap to render to and attach to framebuffer (Put in textures)
			// ---------------------------------------------------------

			m_HDREnvironmentMap->SetupCubemap(512, 512);

			// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
			// ----------------------------------------------------------------------------------------------

			glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
			glm::mat4 captureViews[] =
			{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
			};

			// pbr: convert HDR equirectangular environment map to cubemap equivalent (texture)
			// ----------------------------------------------------------------------

			std::dynamic_pointer_cast<ge::OpenGLShader>(equirectangularToCubemapShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(equirectangularToCubemapShader)->UploadUniformInt("u_EquirectangularMap", 0);
			std::dynamic_pointer_cast<ge::OpenGLShader>(equirectangularToCubemapShader)->UploadUniformMat4("u_Projection", captureProjection);

			m_HDREnvironmentMap->Bind(0);

			ge::RenderCommand::SetViewport(0, 0, 512, 512);

			setupCube();

			m_Framebuffer->Bind();
			for (unsigned int i = 0; i < 6; ++i) 
			{
				std::dynamic_pointer_cast<ge::OpenGLShader>(equirectangularToCubemapShader)->UploadUniformMat4("u_View", captureViews[i]);
				m_Framebuffer->AttachCubemapTexture(m_HDREnvironmentMap->GetCubemapID(), i);
				ge::RenderCommand::Clear();

				ge::Renderer::SubmitFramebuffer(m_CubeVA, 36);
			}

			m_Framebuffer->Unbind();

			// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
			m_HDREnvironmentMap->GenerateMipmap();

			// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
			// -------------------------------------------------------------------------------

			m_HDREnvironmentMap->SetupIrradianceMap(32, 32);

			m_Framebuffer->Rescale(32, 32);

			// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
			// -----------------------------------------------------------------------------

			std::dynamic_pointer_cast<ge::OpenGLShader>(irradianceShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(irradianceShader)->UploadUniformInt("u_EnvironmentMap", 0);
			std::dynamic_pointer_cast<ge::OpenGLShader>(irradianceShader)->UploadUniformMat4("u_Projection", captureProjection);

			m_HDREnvironmentMap->BindCubemap(0);

			ge::RenderCommand::SetViewport(0, 0, 32, 32);

			m_Framebuffer->Bind();
			for (unsigned int i = 0; i < 6; ++i)
			{
				std::dynamic_pointer_cast<ge::OpenGLShader>(irradianceShader)->UploadUniformMat4("u_View", captureViews[i]);
				m_Framebuffer->AttachCubemapTexture(m_HDREnvironmentMap->GetIrradianceID(), i);
				ge::RenderCommand::Clear();

				ge::Renderer::SubmitFramebuffer(m_CubeVA, 36);
			}

			m_Framebuffer->Unbind();

			// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
			// --------------------------------------------------------------------------------
			m_HDREnvironmentMap->SetupPrefilterMap(128, 128);

			// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
			// ----------------------------------------------------------------------------------------------------

			std::dynamic_pointer_cast<ge::OpenGLShader>(prefilterShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(prefilterShader)->UploadUniformInt("u_EnvironmentMap", 0);
			std::dynamic_pointer_cast<ge::OpenGLShader>(prefilterShader)->UploadUniformMat4("u_Projection", captureProjection);

			m_HDREnvironmentMap->BindCubemap(0);

			m_Framebuffer->Bind();
			unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			{
				// resize framebuffer according to mip-level size
				unsigned int mipWidth = 128 * std::pow(0.5, mip);
				unsigned int mipHeight = 128 * std::pow(0.5, mip);
				m_Framebuffer->ResizeRenderBuffer(mipWidth, mipHeight);
				ge::RenderCommand::SetViewport(0, 0, mipWidth, mipHeight);

				float roughness = (float)mip / (float)(maxMipLevels - 1);
				std::dynamic_pointer_cast<ge::OpenGLShader>(prefilterShader)->UploadUniformFloat("u_Roughness", roughness);
				for (unsigned int i = 0; i < 6; ++i)
				{
					std::dynamic_pointer_cast<ge::OpenGLShader>(prefilterShader)->UploadUniformMat4("u_View", captureViews[i]);
					m_Framebuffer->AttachCubemapTexture(m_HDREnvironmentMap->GetPrefilterID(), i, mip);
					ge::RenderCommand::Clear();

					ge::Renderer::SubmitFramebuffer(m_CubeVA, 36);
				}
			}

			m_Framebuffer->Unbind();
	
			// pbr: generate a 2D LUT from the BRDF equations used.
			// ----------------------------------------------------

			m_HDREnvironmentMap->SetupBrdfLUTTexture(512, 512);

			m_Framebuffer->Rescale(512, 512);
			m_Framebuffer->Attach2DTexture(m_HDREnvironmentMap->GetBrdfLUTTextureID());

			ge::RenderCommand::SetViewport(0, 0, 512, 512);
			std::dynamic_pointer_cast<ge::OpenGLShader>(brdfShader)->Bind();
			ge::RenderCommand::Clear();
			setupQuad();
			ge::Renderer::SubmitFramebuffer(m_QuadVA, 4);
			m_Framebuffer->Unbind();
			
			// then before rendering, configure the viewport to the original framebuffer's screen dimensions
			ge::RenderCommand::SetViewport(0, 0, 1280, 720);

			//ge::RenderCommand::WireFrame();
		}

		// Code for 2D scene
		else if (m_Scene == Scene::Scene2D) 
		{
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
	}

	// Function called every frame
	void OnUpdate(ge::DeltaTime dt) override
	{
		// Code for 3D scene
		if (m_Scene == Scene::Scene3D)
		{
			// Update Camera
			m_PerspectiveCameraController.OnUpdate(dt);

			// Rendering
			// Clear previous frame
			ge::RenderCommand::SetClearColor({ 0.2f, 0.3f, 0.3f, 1 });
			ge::RenderCommand::Clear();

			// Begin the current scene
			ge::Renderer::BeginScene(m_PerspectiveCameraController.GetCamera());

			auto pbrShader = m_ShaderLibrary.Get("PBR5");

			// Set up uniforms
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->Bind();
			std::dynamic_pointer_cast<ge::OpenGLShader>(pbrShader)->UploadUniformFloat3("u_ViewPosition", m_PerspectiveCameraController.GetCameraPosition());

			
			m_HDREnvironmentMap->BindIrradianceMap(0);
			m_HDREnvironmentMap->BindPrefilterMap(1);
			m_HDREnvironmentMap->BindBrdfLUTTexture(2);

			glm::mat4 transform = glm::mat4(1.0f);

			// rusted iron
			m_IronAlbedo->Bind(3);
			m_IronNormal->Bind(4);
			m_IronMetallic->Bind(5);
			m_IronRoughness->Bind(6);
			m_IronAo->Bind(7);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(-7.0, 0.0, 2.0));
			ge::Renderer::Submit(pbrShader, m_PbrVA, transform);

			// gold
			m_GoldAlbedo->Bind(3);
			m_GoldNormal->Bind(4);
			m_GoldMetallic->Bind(5);
			m_GoldRoughness->Bind(6);
			m_GoldAo->Bind(7);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(-4.0, 0.0, 2.0));
			ge::Renderer::Submit(pbrShader, m_PbrVA, transform);

			// grass
			m_GrassAlbedo->Bind(3);
			m_GrassNormal->Bind(4);
			m_GrassMetallic->Bind(5);
			m_GrassRoughness->Bind(6);
			m_GrassAo->Bind(7);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(-1.0, 0.0, 2.0));
			ge::Renderer::Submit(pbrShader, m_PbrVA, transform);

			// plastic
			m_PlasticAlbedo->Bind(3);
			m_PlasticNormal->Bind(4);
			m_PlasticMetallic->Bind(5);
			m_PlasticRoughness->Bind(6);
			m_PlasticAo->Bind(7);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(2.0, 0.0, 2.0));
			ge::Renderer::Submit(pbrShader, m_PbrVA, transform);

			// wall
			m_WallAlbedo->Bind(3);
			m_WallNormal->Bind(4);
			m_WallMetallic->Bind(5);
			m_WallRoughness->Bind(6);
			m_WallAo->Bind(7);

			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, glm::vec3(5.0, 0.0, 2.0));
			ge::Renderer::Submit(pbrShader, m_PbrVA, transform);

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

			// render skybox(render as last to prevent overdraw)
			auto backgroundShader = m_ShaderLibrary.Get("Background");
			m_HDREnvironmentMap->BindCubemap(0);
			ge::Renderer::SubmitSkybox(backgroundShader, m_CubeVA, 36);

			ge::Renderer::EndScene();
		}

		// Code for 2D scene
		else if (m_Scene == Scene::Scene2D) 
		{
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

	void setupCube()
	{
		m_CubeVA.reset(ge::VertexArray::Create());
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		// Set buffer layout for cube
		ge::Ref<ge::VertexBuffer> cubeVB;
		cubeVB.reset(ge::VertexBuffer::Create(vertices, sizeof(vertices)));

		cubeVB->SetLayout({
			{ ge::ShaderDataType::Float3, "a_Position" },
			{ ge::ShaderDataType::Float3, "a_Normal" },
			{ ge::ShaderDataType::Float2, "a_TexCoord" }
			});

		m_CubeVA->AddVertexBuffer(cubeVB);
	}

	void setupQuad()
	{
		m_QuadVA.reset(ge::VertexArray::Create());
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		// Set buffer layout for quad
		ge::Ref<ge::VertexBuffer> quadVB;
		quadVB.reset(ge::VertexBuffer::Create(quadVertices, sizeof(quadVertices)));

		quadVB->SetLayout({
			{ ge::ShaderDataType::Float3, "a_Position" },
			{ ge::ShaderDataType::Float2, "a_TexCoord" }
		});

		m_QuadVA->AddVertexBuffer(quadVB);
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

	ge::Ref<ge::VertexArray> m_PbrVA, m_CubeVA, m_QuadVA;

	ge::Ref<ge::Texture2D> m_IronAlbedo, m_IronNormal, m_IronMetallic, m_IronRoughness, m_IronAo;
	ge::Ref<ge::Texture2D> m_GoldAlbedo, m_GoldNormal, m_GoldMetallic, m_GoldRoughness, m_GoldAo;
	ge::Ref<ge::Texture2D> m_GrassAlbedo, m_GrassNormal, m_GrassMetallic, m_GrassRoughness, m_GrassAo;
	ge::Ref<ge::Texture2D> m_PlasticAlbedo, m_PlasticNormal, m_PlasticMetallic, m_PlasticRoughness, m_PlasticAo;
	ge::Ref<ge::Texture2D> m_WallAlbedo, m_WallNormal, m_WallMetallic, m_WallRoughness, m_WallAo;

	std::vector<glm::vec3> m_LightPositions;
	std::vector<glm::vec3> m_LightColors;

	ge::Ref<ge::Framebuffer> m_Framebuffer;

	ge::Ref<ge::HDREnvironmentMap> m_HDREnvironmentMap;

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