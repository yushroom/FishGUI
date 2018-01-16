#include <FishGUI/Window.hpp>
#include <FishGUI/Input.hpp>

#include <cassert>
#include <iostream>
#include <GLFW/glfw3.h>

#include <nanovg.h>
#include <FishGUI/Draw.hpp>

namespace FishGUI
{
	void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		
		auto type = action==GLFW_PRESS ? KeyEvent::Type::KeyPress : KeyEvent::Type::KeyRelease;
		static KeyEvent e(type, key, mods);
		auto w = WindowManager::GetInstance().FindWindow(window);
		w->OnKeyEvent(&e);
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
//		printf("mods: %d\n", mods);
		MouseButtonState state = MouseButtonState::None;
		auto win = WindowManager::GetInstance().FindWindow(window);
		auto& input = win->GetInput();
		if (action == GLFW_PRESS)
		{
			//printf("GLFW_PRESS\n");
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
			//printf("GLFW_RELEASE\n");
			state = MouseButtonState::Up;
		}
		
		input.m_mouseButtonStates[button] = state;
		input.m_mouseEventModifiers[button] = mods;
		
		auto type = action==GLFW_PRESS ? MouseEvent::Type::MouseButtonPress : MouseEvent::Type::MouseButtonRelease;
		auto btn = static_cast<MouseButton>(button);
		auto e = new MouseEvent(type, input.GetMousePosition(), btn, mods);
		input.m_mouseEvents.push_back(e);
//		win->OnMouseEvent(&e);
	}
	
	void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
//		printf("xoffset: %le yoffset: %le\n", xoffset, yoffset);
		auto& input = WindowManager::GetInstance().FindWindow(window)->GetInput();
		input.m_scrolling = true;
		input.m_scroll.x = static_cast<float>(xoffset*4);
		input.m_scroll.y = static_cast<float>(yoffset*4);
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
		: m_name(title), m_size{ width, height }, m_context(context)
	{
		m_input.m_window = this;
		m_minSize.width = 200;
		m_minSize.height = 200;
//		m_rect.width = width;
//		m_rect.height = height;
		
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
	
	
	void Window::BeforeFrame()
	{
		Context::GetInstance().BindWindow(this);
		glfwMakeContextCurrent(m_glfwWindow);
		
		m_isFocused = (glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED) == 1);
		
		int width = m_size.width;
		int height = m_size.height;
		int fbWidth = m_size.width;
		int fbHeight = m_size.height;
		glfwGetWindowSize(m_glfwWindow, &width, &height);
		glfwGetFramebufferSize(m_glfwWindow, &fbWidth, &fbHeight);
		
		m_size.width = width;
		m_size.height = height;
		
		m_widgets.clear();
		
		glViewport(0, 0, fbWidth, fbHeight);
		float bck = 162 / 255.0f;
		glClearColor(bck, bck, bck, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		float ratio = float(fbWidth) / width;
		
		PreDraw();
//		glfwSwapBuffers(m_glfwWindow);
		
		glViewport(0, 0, fbWidth, fbHeight);
		glClearColor(bck, bck, bck, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		Context::GetInstance().m_drawContext->vg = this->GetNVGContext();
		Context::GetInstance().m_input = &m_input;
		nvgBeginFrame(GetNVGContext(), width, height, ratio);
//		Image(9, Rect{0, 0, 800, 600});
	}
	
	Widget* FindVisableWidget(std::vector<Widget*>& widgets, int x, int y)
	{
		for (auto it = widgets.rbegin(); it != widgets.rend(); ++it)
		{
			auto w = *it;
			if (PointInRect(x, y, w->GetRect()))
			{
				return w;
			}
		}
		return nullptr;
	}
	
	void Window::OnMouseEvent(MouseEvent* e)
	{
		Widget* newFocused = nullptr;
		if (e->type() == MouseEvent::Type::MouseButtonPress)
		{
			if (e->button() == MouseButton::Left)
			{
				auto p = e->pos();
				newFocused = FindVisableWidget(m_widgets, p.x, p.y);
			}
			if (newFocused != nullptr)
			{
				for (auto w : m_widgets)
				{
					w->SetIsFocused(false);
				}
				newFocused->SetIsFocused(true);
				m_focusedWidget = newFocused;
			}
		}
		
		if (m_focusedWidget != nullptr)
		{
			m_focusedWidget->OnMouseEvent(e);
		}
	}
	
	void Window::OnKeyEvent(KeyEvent* e)
	{
		if (m_focusedWidget != nullptr)
		{
			m_focusedWidget->OnKeyEvent(e);
		}
	}

	
	void Window::AfterFrame()
	{
		Context::GetInstance().UnbindWindow();

		if (!m_isFocused)
		{
			for (auto w : m_widgets)
			{
				w->SetIsFocused(false);
			}
		}
		else
		{
			if (m_focusedWidget != nullptr)
				m_focusedWidget->SetIsFocused(true);
		}
		
		nvgEndFrame(GetNVGContext());
		OverlayDraw();
		glfwSwapBuffers(m_glfwWindow);
	}
	
	void Window::Draw()
	{
		int iconified = glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED);
		if (iconified)
			return;
		BeforeFrame();
		//		Widget::Draw();
		if (m_layout != nullptr)
		{
			Rect rect = {0, 0, m_size.width, m_size.height};
			m_layout->PerformLayout(rect);
		}
		
		AfterFrame();
	}
	
	
	MainWindow::MainWindow(FishGUIContext* context, const char* title, int width, int height)
	: Window(context, title, width, height)
	{
		m_minSize.width = 950;
		m_minSize.height = 600;
		glfwSetWindowSizeLimits(m_glfwWindow, m_minSize.width, m_minSize.height, m_maxSize.width, m_maxSize.height);
	}
	
	void MainWindow::Draw()
	{
		int iconified = glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED);
		if (iconified)
			return;
		
		BeforeFrame();
		
		Rect rect = {0, 0, m_size.width, m_size.height};
		
		if (m_toolBar != nullptr)
		{
			m_toolBar->SetRect(0, 0, rect.width, m_toolBar->GetHeight());
			m_toolBar->BindAndDraw();
		}
		
		if (m_statusBar != nullptr)
		{
			m_statusBar->SetRect(0, rect.height - m_statusBar->GetHeight(), rect.width, m_statusBar->GetHeight());
			m_statusBar->BindAndDraw();
		}
		if (m_toolBar != nullptr)
		{
			rect.height -= m_toolBar->GetHeight();
			rect.y += m_toolBar->GetHeight();
		}
		if (m_statusBar != nullptr)
		{
			rect.height -= m_statusBar->GetHeight();
		}
		
		//		Widget::Draw();
		if (m_layout != nullptr)
		{
			m_layout->PerformLayout(rect);
		}
		
		if (m_toolBar != nullptr)
		{
			rect.height += m_toolBar->GetHeight();
			rect.y -= m_toolBar->GetHeight();
		}
		if (m_statusBar != nullptr)
		{
			rect.height += m_statusBar->GetHeight();
		}
		
		AfterFrame();
	}

}
