#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>

#include "RFGUI.hpp"
#ifdef _WIN32
	#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
	window = glfwCreateWindow(800, 600, "Fish GUI", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

#ifdef _WIN32
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	RFGUI::Init(window);
	
	std::string email = "mail@example.com";
	
	auto f1 = [&email]() {
		RFGUI::CheckBox("CheckBox", nullptr);
		RFGUI::Label("Login");
		
		RFGUI::InputText("Email", email);
		float value = 0.4f;
		RFGUI::Slider("Diameter", &value, 0.0f, 1.0f);
		if (RFGUI::Button("button 2")) {
			printf("button 2 clicked\n");
		}
	};
	
	auto f2 = [&email]() {
		RFGUI::InputText("Email1234567", email);
		RFGUI::Label("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQq");
		if (RFGUI::Button("button 1 1234567890111213")) {
			printf("button 1 clicked\n");
//			pos4 = RFGUI::TabPosition::Floating;
		}
		if (RFGUI::Button("button 2")) {
			printf("button 2 clicked\n");
		}
	};
	
	RFGUI::Tab* top    = RFGUI::CreateTab("Top", RFGUI::TabPosition::Top, 60);
	top->m_resizable = false;
	RFGUI::Tab* right  = RFGUI::CreateTab("Right", RFGUI::TabPosition::Right, 280);
	RFGUI::Tab* bottom = RFGUI::CreateTab("Bottom", RFGUI::TabPosition::Bottom, 200);
	RFGUI::Tab* left   = RFGUI::CreateTab("Left", RFGUI::TabPosition::Left, 160);
	
	right->m_renderFunction = f1;
	bottom->m_renderFunction = f1;
	left->m_renderFunction = f2;
	
	while (!glfwWindowShouldClose(window))
	{
		RFGUI::Begin();
		RFGUI::End();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

