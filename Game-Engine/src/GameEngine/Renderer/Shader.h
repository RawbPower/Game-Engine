#pragma once

#include <string>

// This will be abstracted in the future
namespace ge {

	class Shader {
	public:
		// Will take a file as argument in the future. Also fragment shader is pixel shader in OpenGL
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID;		// Number that identifies this object in OpenGL
	};
}
