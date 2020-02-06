/*
	OpenGL Texture

	Class for texture in OpenGL API
*/

#include "gepch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include <glad/glad.h>

namespace ge {

	// Load textures from file
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool gammaCorrection)
		: m_Path(path)
	{
		glGenTextures(1, &m_RendererID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum internalFormat;
			GLenum dataFormat;
			if (nrComponents == 1)
			{
				internalFormat = dataFormat = GL_RED;
			}
			else if (nrComponents == 3)
			{
				internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
				dataFormat = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
				dataFormat = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
			GE_CORE_INFO(path + " loaded");
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		if (slot == 0)
		{
			glActiveTexture(GL_TEXTURE0);
		}
		else if (slot == 1)
		{
			glActiveTexture(GL_TEXTURE1);
		}
		else if (slot == 2)
		{
			glActiveTexture(GL_TEXTURE2);
		}
		else if (slot == 3)
		{
			glActiveTexture(GL_TEXTURE3);
		}
		else if (slot == 4)
		{
			glActiveTexture(GL_TEXTURE4);
		}
		else if (slot == 5)
		{
			glActiveTexture(GL_TEXTURE5);
		}
		else if (slot == 6)
		{
			glActiveTexture(GL_TEXTURE6);
		}
		else if (slot == 7)
		{
			glActiveTexture(GL_TEXTURE7);
		}
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}


	// Load 3D texture for a model
	OpenGLTexture3D::OpenGLTexture3D(const std::string& path, const std::string& directory)
		: m_Path(path)
	{
		// Load Texture
		std::string filename = std::string(path);
		filename = directory + '/' + filename;
		GE_CORE_TRACE(filename);
		glGenTextures(1, &m_RendererID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		m_Width = width;
		m_Height = height;
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
			GE_CORE_INFO(path + " loaded");
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}
	}

	OpenGLTexture3D::~OpenGLTexture3D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture3D::Bind(uint32_t slot) const
	{
		if (slot == 0)
		{
			glActiveTexture(GL_TEXTURE0);
		}
		else if (slot == 1)
		{
			glActiveTexture(GL_TEXTURE1);
		}
		else if (slot == 2)
		{
			glActiveTexture(GL_TEXTURE2);
		}
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}



	// Load cubemap texture
	OpenGLCubemap::OpenGLCubemap(const std::vector<std::string> faces)
		: m_FacePaths(faces)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		// Load image
		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			stbi_uc* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
			if (data)
			{
				m_Width = width;
				m_Height = height;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				GE_CORE_ASSERT(data, "Failed to load cubmap image!");
			}
		}

		// Defining parameters (for scaling)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLCubemap::~OpenGLCubemap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubemap::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	// Load HDR Environment Map texture
	OpenGLHDREnvironmentMap::OpenGLHDREnvironmentMap(const std::string& path)
		: m_Path(path)
	{
		// Load image
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
		if (data)
		{
			m_Width = width;
			m_Height = height;

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

			// Defining parameters (for scaling)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);

			GE_CORE_INFO(path + " loaded");
		}
		else
		{
			GE_CORE_ASSERT(data, "Failed to load HDR image!");
		}
	}

	OpenGLHDREnvironmentMap::~OpenGLHDREnvironmentMap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLHDREnvironmentMap::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void OpenGLHDREnvironmentMap::BindCubemap(uint32_t slot) const
	{
		if (slot == 0)
		{
			glActiveTexture(GL_TEXTURE0);
		}
		else if (slot == 1)
		{
			glActiveTexture(GL_TEXTURE1);
		}
		else if (slot == 2)
		{
			glActiveTexture(GL_TEXTURE2);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);
	}

	void OpenGLHDREnvironmentMap::BindIrradianceMap(uint32_t slot) const
	{
		if (slot == 0)
		{
			glActiveTexture(GL_TEXTURE0);
		}
		else if (slot == 1)
		{
			glActiveTexture(GL_TEXTURE1);
		}
		else if (slot == 2)
		{
			glActiveTexture(GL_TEXTURE2);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceID);
	}

	void OpenGLHDREnvironmentMap::BindPrefilterMap(uint32_t slot) const
	{
		if (slot == 0)
		{
			glActiveTexture(GL_TEXTURE0);
		}
		else if (slot == 1)
		{
			glActiveTexture(GL_TEXTURE1);
		}
		else if (slot == 2)
		{
			glActiveTexture(GL_TEXTURE2);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_PrefilterID);
	}

	void OpenGLHDREnvironmentMap::BindBrdfLUTTexture(uint32_t slot) const
	{
		if (slot == 0)
		{
			glActiveTexture(GL_TEXTURE0);
		}
		else if (slot == 1)
		{
			glActiveTexture(GL_TEXTURE1);
		}
		else if (slot == 2)
		{
			glActiveTexture(GL_TEXTURE2);
		}
		glBindTexture(GL_TEXTURE_2D, m_BrdfLUTTextureID);
	}

	void OpenGLHDREnvironmentMap::GenerateMipmap() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}


	void OpenGLHDREnvironmentMap::SetupCubemap(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &m_CubemapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void OpenGLHDREnvironmentMap::SetupIrradianceMap(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &m_IrradianceID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceID);

		SetMapTextures(width, height);
	}

	void OpenGLHDREnvironmentMap::SetupPrefilterMap(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &m_PrefilterID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_PrefilterID);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // set minifcation filter to mip_linear
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void OpenGLHDREnvironmentMap::SetupBrdfLUTTexture(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &m_BrdfLUTTextureID);
		glBindTexture(GL_TEXTURE_2D, m_BrdfLUTTextureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, 0);

		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void OpenGLHDREnvironmentMap::SetMapTextures(uint32_t width, uint32_t height)
	{
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}