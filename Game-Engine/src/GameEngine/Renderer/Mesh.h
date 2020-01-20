#pragma once

#include <GameEngine/Renderer/Buffer.h>
#include <GameEngine/Renderer/VertexArray.h>
#include <GameEngine/Renderer/Shader.h>

#include <GameEngine/Core/Core.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

// Needs to be abstracted after I test that it works

namespace ge {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Texture
	{
		unsigned int id;
		std::string type;
	};

	class Mesh {
	public: 
		/* Functions */
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
		~Mesh();

		void Draw(const std::shared_ptr<Shader>& shader);
	private:
		/* Mesh Data */
		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
		std::vector<Texture> m_Textures;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		void SetupMesh();
	};

}

