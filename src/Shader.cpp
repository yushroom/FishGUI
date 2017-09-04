#include "Shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
using namespace std;

Shader::Shader(const std::string& vs_path, const std::string ps_path)
{
	std::ifstream vs_stream(vs_path);
	std::ifstream ps_stream(ps_path);
	assert(vs_stream.is_open());
	assert(ps_stream.is_open());
	std::stringstream vs_sstream, ps_sstream;
	vs_sstream << vs_stream.rdbuf();
	ps_sstream << ps_stream.rdbuf();
	string vs_string = vs_sstream.str();
	string ps_string = ps_sstream.str();
	std::cout << vs_string << endl;
	std::cout << ps_string << endl;
	//const GLchar* vs_str = vs_sstream.str().c_str();
	const GLchar* vs_str = vs_string.c_str();
	const GLchar* ps_str = ps_string.c_str();

	GLuint vs, ps;
	GLint success;
	GLchar infoLog[1024];

	// vs
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_str, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, 1024, NULL, infoLog);
		std::cout << infoLog << endl;
		abort();
	}
    
    // ps
    ps = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(ps, 1, &ps_str, NULL);
    glCompileShader(ps);
    glGetShaderiv(ps, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ps, 1024, NULL, infoLog);
        std::cout << infoLog << endl;
        abort();
    }

	m_program = glCreateProgram();
	glAttachShader(m_program, vs);
	glAttachShader(m_program, ps);
	glLinkProgram(m_program);
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_program, 1024, NULL, infoLog);
		std::cout << infoLog << endl;
		abort();
	}
	glDeleteProgram(vs);
	glDeleteProgram(ps);
}


Shader::~Shader()
{
	//glDeleteProgram(m_program);
}
