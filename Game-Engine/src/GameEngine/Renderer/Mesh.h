/*
	Mesh

	Class used to to draw the mesh of a 3D model
*/

#pragma once

#include <GameEngine/Renderer/Buffer.h>
#include <GameEngine/Renderer/VertexArray.h>
#include <GameEngine/Renderer/Shader.h>
#include <GameEngine/Renderer/Texture.h>

#include <GameEngine/Core/Core.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

namespace ge {

	struct MeshVertex
	{
		// position
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoords;
		// tangent
		glm::vec3 Tangent;
		// bitangent
		glm::vec3 Bitangent;
	};

	class Mesh {
	public: 
		/* Functions */
		// Constructor
		Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<Ref<Texture3D>> textures);
		~Mesh();

		// render the mesh
		void Draw(const std::shared_ptr<Shader>& shader);
	private:
		/* Mesh Data */
		std::vector<MeshVertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
		std::vector<Ref<Texture3D>> m_Textures;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		// initializes all the buffer objects/arrays
		void SetupMesh();
	};

}

