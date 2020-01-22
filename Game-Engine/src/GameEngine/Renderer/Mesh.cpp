#include "gepch.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

// Fix this
#include "Platform/OpenGL/OpenGLShader.h"

namespace ge {

	Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures)
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
	/*void Mesh::Draw(const std::shared_ptr<Shader>& shader)
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
	}*/


	void Mesh::Draw(const std::shared_ptr<Shader>& shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (uint32_t i = 0; i < m_Textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = m_Textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformInt((name + number), i);
			glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}


	/*void Mesh::SetupMesh()
	{
		m_VertexArray.reset(ge::VertexArray::Create());

		// Create a vertex buffer for the object
		m_VertexBuffer.reset(ge::VertexBuffer::Create(&m_Vertices[0], m_Vertices.size() * sizeof(MeshVertex)));

		m_VertexBuffer->SetLayout({
			{ ge::ShaderDataType::Float3, "a_Position" },
			{ ge::ShaderDataType::Float3, "a_Normal" },
			{ ge::ShaderDataType::Float2, "a_TexCoords" }
		});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer.reset(ge::IndexBuffer::Create(m_Indices.data(), m_Indices.size()));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}*/


	void Mesh::SetupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(MeshVertex), &m_Vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, TexCoords));

		glBindVertexArray(0);
	}
}