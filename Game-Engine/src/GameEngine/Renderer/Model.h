#pragma once

#include <string>
#include <vector>

#include <GameEngine/Renderer/Shader.h>
#include <GameEngine/Renderer/Mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ge {

	class Model {
	public:
		/* Functions */

		// Constructor, expects path of 3D model
		Model(const std::string& path);
		~Model();

		// Draws the model and thus all the meshes
		void Draw(const std::shared_ptr<Shader>& shader);
	private:
		/* Functions */

		// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void LoadModel(const std::string& path);

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void ProcessNode(aiNode* node, const aiScene* scene);


		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.	
		std::vector<MeshTexture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

		// connect this function bac k to the original texture file
		unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

		/* Model Data */
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;
		std::vector<MeshTexture> m_TexturesLoaded;
	};
}