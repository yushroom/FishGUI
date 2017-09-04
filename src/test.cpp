#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "RFGUI.hpp"

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

	RFGUI::Init(window);

#if 0
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif


	while (!glfwWindowShouldClose(window))
	{
		RFGUI::Begin();
		
		RFGUI::BeginTab(nullptr, 60, RFGUI::TabPosition::Top);
		RFGUI::EndTab();
		
		RFGUI::BeginTab("Right", 280, RFGUI::TabPosition::Right);
		RFGUI::CheckBox("CheckBox", nullptr);
		RFGUI::Label("Login");
		std::string email = "mai@example.com";
		RFGUI::InputText("Email", email);
		float value = 0.4f;
		RFGUI::Slider("Diameter", &value, 0.0f, 1.0f);
		RFGUI::EndTab();
		
		RFGUI::BeginTab("bottom", 200, RFGUI::TabPosition::Bottom);
		RFGUI::EndTab();
		
		RFGUI::BeginTab("Left", 160, RFGUI::TabPosition::Left);
		RFGUI::Label("AaBbCcDdEeFfGg");
		if (RFGUI::Button("button 1")) {
			printf("button 1 clicked\n");
			auto w = glfwCreateWindow(640, 480, "2", NULL, window);
//			glfwMakeContextCurrent(w);
			RFGUI::MakeCurrent(w);
		}
		if (RFGUI::Button("button 2")) {
			printf("button 2 clicked\n");
		}
		RFGUI::EndTab();
		
		RFGUI::End();
		
//		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

