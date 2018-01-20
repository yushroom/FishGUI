#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#define GLFW_INCLUDE_GLCOREARB
#else
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#endif

#include <cstdio>

namespace FishGUI
{
	inline void _checkOpenGLError(const char *file, int line)
	{
		GLenum err = glGetError();

		while (err != GL_NO_ERROR)
		{
			const char* error;

			switch (err)
			{
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			default:                        error = "UNKNOWN_ERROR";
			}

	//		LogError(Format("GL_%1% - %2%:%3%", error, file, line));
			printf("[%d] GL_%s\n", line, error);
			err = glGetError();
		}
	}
}

#define glCheckError() FishGUI::_checkOpenGLError(__FILE__,__LINE__);
