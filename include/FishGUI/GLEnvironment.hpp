#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#define GLFW_INCLUDE_GLCOREARB
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

namespace FishGUI
{
	void _checkOpenGLError(const char *file, int line);
}

#define glCheckError() _checkOpenGLError(__FILE__,__LINE__);
