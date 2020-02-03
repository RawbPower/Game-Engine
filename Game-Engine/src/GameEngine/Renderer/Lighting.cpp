/*
	Light

	Class used to set up different type of lighting.

	- Point Light
	- Directional Light
	- Spot Light
*/

#include "gepch.h"
#include "Lighting.h"

// This needs to be abstracted
#include "Platform/OpenGL/OpenGLShader.h"

namespace ge {

	void PointLight::UploadUniforms(const std::shared_ptr<Shader>& shader, const std::string& name, const glm::vec3& pos, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float constant, const float linear, const float quadratic)
	{
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".position", pos);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".ambient", ambient);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".diffuse", diffuse);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".specular", specular);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat(name + ".constant", constant);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat(name + ".linear", linear);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat(name + ".quadratic", quadratic);
	}


	void DirLight::UploadUniforms(const std::shared_ptr<Shader>& shader, const std::string& name, const glm::vec3& dir, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
	{
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".direction", dir);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".ambient", ambient);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".diffuse", diffuse);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".specular", specular);
	}

	
	void SpotLight::UploadUniforms(const std::shared_ptr<Shader>& shader, const std::string& name, const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float constant, const float linear, const float quadratic, const float cutOff, const float outerCutoff)
	{
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".position", pos);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".direction", dir);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".ambient", ambient);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".diffuse", diffuse);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat3(name + ".specular", specular);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat(name + ".constant", constant);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat(name + ".linear", linear);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat(name + ".quadratic", quadratic);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat(name + ".cutOff", glm::cos(glm::radians(cutOff)));
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat(name + ".outerCutOff", glm::cos(glm::radians(outerCutoff)));
	}

}