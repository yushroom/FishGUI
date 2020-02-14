#pragma once

#include <string>
#include <glm/glm.hpp>

namespace FishGUI
{
	class Shader
	{
	public:
		Shader(const char* vShaderCode, const char* fShaderCode, const char* gShaderCode = nullptr);
		~Shader();

		// activate the shader
		// ------------------------------------------------------------------------
		void Use();
		// utility uniform functions
		// ------------------------------------------------------------------------
		void SetBool(const std::string &name, bool value) const;
		// ------------------------------------------------------------------------
		void SetInt(const std::string &name, int value) const;
		// ------------------------------------------------------------------------
		void SetFloat(const std::string &name, float value) const;
		// ------------------------------------------------------------------------
#if 1
		void setVec2(const std::string &name, const glm::vec2 &value) const;
		void setVec2(const std::string &name, float x, float y) const;
		// ------------------------------------------------------------------------
		void setVec3(const std::string &name, const glm::vec3 &value) const;
		void setVec3(const std::string &name, float x, float y, float z) const;
		// ------------------------------------------------------------------------
		void setVec4(const std::string &name, const glm::vec4 &value) const;
		void setVec4(const std::string &name, float x, float y, float z, float w);
		// ------------------------------------------------------------------------
		void setMat2(const std::string &name, const glm::mat2 &mat) const;
		// ------------------------------------------------------------------------
		void setMat3(const std::string &name, const glm::mat3 &mat) const;
		// ------------------------------------------------------------------------
		void setMat4(const std::string &name, const glm::mat4 &mat) const;
#endif
	private:

		// Noncopyable
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		unsigned int m_program = 0;
	};
}
