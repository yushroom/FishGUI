#include "Window.hpp"
#include "Input.hpp"

#include <cassert>
#include <GLFW/glfw3.h>

namespace FishGUI
{
	void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
	
	void glfwCharCallback(GLFWwindow *window, unsigned int codepoint)
	{
		std::cout << codepoint << std::endl;
		// known bug in vs
		// https://stackoverflow.com/questions/32055357/visual-studio-c-2015-stdcodecvt-with-char16-t-or-char32-t
		//static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
		//std::string u8str = converter.to_bytes(codepoint);
		//std::cout << u8str << std::endl;
		//g.input.m_stringBuffer += u8str;
	}
	
	void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		MouseButtonState state = MouseButtonState::None;
		auto& input = Input::GetInstance();
		if (action == GLFW_PRESS)
		{
			state = MouseButtonState::Down;
			double lastTime = input.m_lastMouseButtonClickTime[button];
			double now = glfwGetTime();
			if (now - lastTime < 0.3)
			{
				input.m_leftMouseButtonDoubleClicked = true;
				input.m_lastMouseButtonClickTime[button] = 0;
			}
			else
			{
				input.m_lastMouseButtonClickTime[button] = now;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			state = MouseButtonState::Up;
		}
		
		input.m_mouseButtonStates[button] = state;
	}
	
	void glfwBindWindowCallbacks(GLFWwindow* window)
	{
		glfwSetKeyCallback(window, glfwKeyCallback);
		glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
		glfwSetCharCallback(window, glfwCharCallback);
//		glfwSetWindowSizeCallback(window, glfwWindowResizeCallback);
//		glfwSetglfwWindowFocusCallback(window, glfwWindowFocusCallback);
	}
	
	Window::Window(FishGUIContext* context, const char* title,  int width, int height) 
		: Widget(context, title), m_size{ width, height }
	{
		m_minSize.width = 200;
		m_minSize.height = 200;
		m_rect.width = width;
		m_rect.height = height;
		m_glfwWindow = glfwCreateWindow(800, 600, "Fish GUI", NULL, NULL);
		if (!m_glfwWindow)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		
		glfwBindWindowCallbacks(m_glfwWindow);
		glfwSetWindowSizeLimits(m_glfwWindow, m_minSize.width, m_minSize.height, m_maxSize.width, m_maxSize.height);
	}
	
	void Window::BeforeFrame()
	{
		
	}

	MainWindow::MainWindow(FishGUIContext* context, const char* title, int width, int height)
		: Window(context, title, width, height), m_toolBar(context), m_statusBar(context)
	{
		m_minSize.width = 600;
		m_minSize.height = 400;
		glfwSetWindowSizeLimits(m_glfwWindow, m_minSize.width, m_minSize.height, m_maxSize.width, m_maxSize.height);
	}
}
