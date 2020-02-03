/*
	Light

	Class used to set up different type of lighting.
	
	- Point Light
	- Directional Light
	- Spot Light
*/

#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Shader.h"

namespace ge {

	// Class for uplaoding point light properties to shader
	class PointLight {
	public:
		PointLight()
		{
		}

		~PointLight()
		{
		}

		static void UploadUniforms(const std::shared_ptr<Shader>& shader, const std::string& name, const glm::vec3& pos, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float constant, const float linear, const float quadratic);
	private:
	};

	// Class for uplaoding dirdetional light properties to shader
	class DirLight {
	public:
		DirLight()
		{
		}

		~DirLight()
		{
		}

		static void UploadUniforms(const std::shared_ptr<Shader>& shader, const std::string& name, const glm::vec3& dir, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular);
	private:
	};

	// Class for uplaoding spot light properties to shader
	class SpotLight {
	public:
		SpotLight()
		{
		}

		~SpotLight()
		{
		}

		static void UploadUniforms(const std::shared_ptr<Shader>& shader, const std::string& name, const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float constant, const float linear, const float quadratic, const float cutOff, const float outerCutoff);
	private:
	};


}
