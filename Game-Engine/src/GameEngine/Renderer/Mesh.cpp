#include "gepch.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

// Fix this
#include "Platform/OpenGL/OpenGLShader.h"

namespace ge {

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		m_Vertices = vertices;
		m_Indices = indices;
		m_Textures = textures;

		SetupMesh();
	}

	Mesh::~Mesh()
	{

	}


	// This needs to be abstracted
	void Mesh::Draw(const std::shared_ptr<Shader>& shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (uint32_t i = 0; i < m_Textures.size(); i++)
		{
			std::string number;
			std::string name = m_Textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformInt(("material." + name + number).c_str(), i);
			glBindTextureUnit(i, m_Textures[i].id);
		}
		m_VertexArray->Bind();
		glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);	
	}

	void Mesh::SetupMesh()
	{
		m_VertexArray.reset(ge::VertexArray::Create());

		// Create a vertex buffer for the object
		m_VertexBuffer.reset(ge::VertexBuffer::Create(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex)));

		m_VertexBuffer->SetLayout({
			{ ge::ShaderDataType::Float3, "a_Position" },
			{ ge::ShaderDataType::Float3, "a_Normal" },
			{ ge::ShaderDataType::Float2, "a_TexCoords" }
		});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer.reset(ge::IndexBuffer::Create(m_Indices.data(), m_Indices.size()));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}
}