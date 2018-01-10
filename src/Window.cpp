#include "Window.hpp"
#include "Input.hpp"

#include <cassert>
#include <iostream>
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
		auto& input = WindowManager::GetInstance().FindWindow(window)->GetInput();
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
	
	void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
//		printf("xoffset: %le yoffset: %le\n", xoffset, yoffset);
		auto& input = WindowManager::GetInstance().FindWindow(window)->GetInput();
		input.m_scrolling = true;
		input.m_scroll.x = static_cast<float>(xoffset);
		input.m_scroll.y = static_cast<float>(yoffset);
	}
	
	void glfwBindWindowCallbacks(GLFWwindow* window)
	{
		glfwSetKeyCallback(window, glfwKeyCallback);
		glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
		glfwSetScrollCallback(window, glfwScrollCallback);
		glfwSetCharCallback(window, glfwCharCallback);
//		glfwSetWindowSizeCallback(window, glfwWindowResizeCallback);
//		glfwSetglfwWindowFocusCallback(window, glfwWindowFocusCallback);
	}
	
	Window::Window(FishGUIContext* context, const char* title,  int width, int height)
		: Widget(title), m_size{ width, height }, m_context(context)
	{
		m_minSize.width = 200;
		m_minSize.height = 200;
		m_rect.width = width;
		m_rect.height = height;
		
		GLFWwindow* mainGLFWWindow = nullptr;
		Window* mainWindow = WindowManager::GetInstance().GetMainWindow();
		if (mainWindow != nullptr)
			mainGLFWWindow = mainWindow->GetGLFWWindow();
		
		m_glfwWindow = glfwCreateWindow(width, height, title, nullptr, mainGLFWWindow);
		if (!m_glfwWindow)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		
		glfwBindWindowCallbacks(m_glfwWindow);
		glfwSetWindowSizeLimits(m_glfwWindow, m_minSize.width, m_minSize.height, m_maxSize.width, m_maxSize.height);
	}
	
	Window::~Window()
	{
		glfwDestroyWindow(m_glfwWindow);
	}
	

	MainWindow::MainWindow(FishGUIContext* context, const char* title, int width, int height)
		: Window(context, title, width, height)
	{
		m_minSize.width = 950;
		m_minSize.height = 600;
		glfwSetWindowSizeLimits(m_glfwWindow, m_minSize.width, m_minSize.height, m_maxSize.width, m_maxSize.height);
	}
	
	void Window::SetPosition(int x, int y)
	{
		assert(m_glfwWindow != nullptr);
		glfwSetWindowPos(m_glfwWindow, x, y);
		m_position.x = x;
		m_position.y = y;
	}
	
	void Window::SetSize(int width, int height)
	{
		assert(m_glfwWindow != nullptr);
		glfwSetWindowSize(m_glfwWindow, width, height);
		m_size.width = width;
		m_size.height = height;
	}
	
	void Window::SetTitle(const char* title)
	{
		assert(m_glfwWindow != nullptr);
		m_name = title;
		glfwSetWindowTitle(m_glfwWindow, title);
	}
	
	
	void Window::SetDecorated(bool decorated)
	{
		assert(m_glfwWindow != nullptr);
		int v = decorated ? GLFW_TRUE : GLFW_FALSE;
		glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, v);
	}
}
