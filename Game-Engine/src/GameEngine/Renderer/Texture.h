/*
	Texture

	Abstract class for texture inherited by classes for each Renderering API
*/

#pragma once

#include <string>
#include <vector>

#include "GameEngine/Core/Core.h"

namespace ge {

	// Virtaul abstract base class for other texture classic to inherit from
	class Texture 
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
	};

	// 2D texture (still abstract, it needs to be implemented by specific renderer API's)
	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path, bool gammaCorrection = false);
	};


	// 3D texture
	class Texture3D : public Texture
	{
	public:
		static Ref<Texture3D> Create(const std::string& path, const std::string& directory);

		virtual const std::string& GetPath() const = 0;

		virtual std::string GetType() const = 0;
		virtual void SetType(const std::string& type) = 0;
	};

	// Cubemap
	class Cubemap : public Texture
	{
	public:
		static Ref<Cubemap> Create(const std::vector<std::string> faces);
	};

	// HDR Environment Map
	class HDREnvironmentMap : public Texture
	{
	public:
		static Ref<HDREnvironmentMap> Create(const std::string& path);

		virtual void SetupCubemap(uint32_t width, uint32_t height) = 0;
		virtual void SetupIrradianceMap(uint32_t width, uint32_t height) = 0;

		virtual void BindCubemap(uint32_t slot = 0) const = 0;
		virtual void BindIrradianceMap(uint32_t slot = 0) const = 0;

		virtual uint32_t GetRendererID() const = 0;
		virtual uint32_t GetCubemapID() const = 0;
		virtual uint32_t GetIrradianceID() const = 0;
	private:
		virtual void SetMapTextures(uint32_t width, uint32_t height) = 0;
	};

}
