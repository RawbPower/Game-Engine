/*
	Model

	Class used to load and process a 3D model using ASSIMP
*/

#include <gepch.h>
#include "Model.h"

#include <glm/glm.hpp>

#include "stb_image.h"

namespace ge {

	Model::Model(const std::string& path)
	{
		LoadModel(path);
	}

	Model::~Model()
	{

	}

	void Model::Draw(const std::shared_ptr<Shader>& shader)
	{
		// Draw each mesh
		for (unsigned int i = 0; i < m_Meshes.size(); i++)
			m_Meshes[i].Draw(shader);
	}

	void Model::LoadModel(const std::string& path)
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		// Check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			return;
		}
		// retrieve the directory path of the filepath
		m_Directory = path.substr(0, path.find_last_of('/'));

		// Process ASSIMP's root nodes recursively
		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		// Process all the node's meshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}
		// Then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill
		std::vector<MeshVertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Ref<Texture3D>> textures;

		// Walk through each of the meshes vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			glm::vec3 vector;
			// Process vertex positions
			MeshVertex vertex;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// Process normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// Process Textures
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else 
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);
		}

		// Process Indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// Process Materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		std::vector<Ref<Texture3D>> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Ref<Texture3D>> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Ref<Texture3D>> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Ref<Texture3D>> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures);
	}

	std::vector<Ref<Texture3D>> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Ref<Texture3D>> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			GE_CORE_TRACE(str.C_Str());
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
			{
				if (std::strcmp(m_TexturesLoaded[j]->GetPath().data(), str.C_Str()) == 0)
				{
					textures.push_back(m_TexturesLoaded[j]);
					skip = true;	// a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)		// If the texture hasn't been already loaded, load it
			{
				Ref<Texture3D> texture = Texture3D::Create(str.C_Str(), this->m_Directory);
				texture->SetType(typeName);
				textures.push_back(texture);
				m_TexturesLoaded.push_back(texture);	// add to loaded textures
			}
		}
		return textures;
	}
}