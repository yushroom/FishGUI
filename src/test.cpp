#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "RFGUI.hpp"
#include "RFFont.hpp"
#include "Bitmap.hpp"
#include "Shader.hpp"

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

GLfloat vertices[] = {
	0.5f,  0.5f, 0.0f,  // Top Right
	0.5f, -0.5f, 0.0f,  // Bottom Right
	-0.5f, -0.5f, 0.0f,  // Bottom Left
	-0.5f,  0.5f, 0.0f   // Top Left 
};

GLuint indices[] = {  // Note that we start from 0!
	0, 1, 3,  // First Triangle
	1, 2, 3   // Second Triangle
};

GLuint VBO, VAO, EBO;

void init()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}


int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	RFGUI::init(window, 640, 480);
	glfwSetWindowSizeCallback(window, RFGUI::window_size_callback);
	glfwSetKeyCallback(window, RFGUI::key_callback);
	glfwSetCursorPosCallback(window, RFGUI::cursor_position_callback);
	glfwSetMouseButtonCallback(window, RFGUI::mouse_button_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	float ratio;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;
	glViewport(0, 0, width, height);

	// auto bmp = RFFont::get_instance()->get_bitmap_for_char('g', 512);
	// bmp->save("./fonts.bmp");

	//GLuint frame_buffer, color_buffer;
	//glGenRenderbuffers(1, &color_buffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, color_buffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

	//GLuint fbo;
	//glGenFramebuffers(1, &fbo);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_buffer);
	//assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	std::vector<uint8_t> pixels(width*height*3, 0);

	auto shader = Shader("../shader/simple.vs", "../shader/simple.fs");
	init();

	while (!glfwWindowShouldClose(window))
	{
		glViewport(0, 0, width, height);

		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glClearColor(0, 1, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//glOrtho(0.f, 1.f, 0.f, 1.f, 1.f, -1.f);

		//RFGUI::Begin();
		//RFGUI::SideBar();

		//RFGUI::Label("AaBbCcDdEeFfGg");

		//if (RFGUI::Button("button 1")) {
		//	printf("button 1 clicked\n");
		//}

		//if (RFGUI::Button("button 2")) {
		//	printf("button 2 clicked\n");
		//}

		//RFGUI::End();


		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
		//write_bmp("pixels.bmp", width, height, pixels);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

