#include "gepch.h"
#include "Mesh.h"
#include "GameEngine/Core/Log.h"

#include <glm/glm.hpp>

// Fix this
#include "Platform/OpenGL/OpenGLShader.h"

#include "GameEngine/Renderer/RenderCommand.h"

namespace ge {

	Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<Ref<Texture3D>> textures)
	{
		m_Vertices = vertices;
		m_Indices = indices;
		m_Textures = textures;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		SetupMesh();
	}

	Mesh::~Mesh()
	{

	}


	// This needs to be abstracted
	void Mesh::Draw(const std::shared_ptr<Shader>& shader)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (uint32_t i = 0; i < m_Textures.size(); i++)
		{
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = m_Textures[i]->GetType();
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);
			GE_CORE_INFO("{0}, {1}, {2}, {3}", diffuseNr, specularNr, normalNr, heightNr);
			// now set the sampler to the correct texture unit
			std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformInt((name + number).c_str(), i);
			// and finally bind the texture
			m_Textures[i]->Bind(i);
		}
		// draw mesh
		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_Indices);	
	}


	void Mesh::SetupMesh()
	{
		// create vertex array
		m_VertexArray.reset(ge::VertexArray::Create());

		// Create a vertex buffer for the object
		m_VertexBuffer.reset(ge::VertexBuffer::Create(&m_Vertices[0], m_Vertices.size() * sizeof(MeshVertex)));

		// Set the layout of the buffer
		m_VertexBuffer->SetLayout({
			{ ge::ShaderDataType::Float3, "a_Position" },
			{ ge::ShaderDataType::Float3, "a_Normal" },
			{ ge::ShaderDataType::Float2, "a_TexCoords" },
			{ ge::ShaderDataType::Float3, "a_Tangent" },
			{ ge::ShaderDataType::Float3, "a_Bitangent" }
		});

		// Add the vertex buffer to the vertex array
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		// Add index buffer to vertex array
		m_IndexBuffer.reset(ge::IndexBuffer::Create(m_Indices.data(), m_Indices.size()));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

}