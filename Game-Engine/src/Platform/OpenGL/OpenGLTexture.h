#pragma once

#include "GameEngine/Renderer/Texture.h"

namespace ge {

	class OpenGLTexture2D : public Texture2D 
	{
	public:
		OpenGLTexture2D(const std::string& path, bool gammaCorrection);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot = 0) const override;
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};


	class OpenGLTexture3D : public Texture3D
	{
	public:
		OpenGLTexture3D(const std::string& path, const std::string& directory);
		virtual ~OpenGLTexture3D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual const std::string& GetPath() const override { return m_Path; }

		virtual std::string GetType() const override { return m_Type; }
		virtual void SetType(const std::string& type) override { m_Type = type; }

		virtual void Bind(uint32_t slot = 0) const override;
	private:
		std::string m_Path;
		std::string m_Type;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};


	class OpenGLCubemap : public Cubemap
	{
	public:
		OpenGLCubemap(const std::vector<std::string> faces);
		virtual ~OpenGLCubemap();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot = 0) const override;
	private:
		std::vector<std::string> m_FacePaths;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};

}
