#pragma once

#include "RFGUI.hpp"
#include <string>

class Shader
{
public:
	Shader(const std::string& vs_path, const std::string ps_path);
	~Shader();

	void Use() {
		glUseProgram(this->m_program);
	}

private:
	GLuint m_program;
};

