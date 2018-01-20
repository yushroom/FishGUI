#include <FishGUI/GLEnvironment.hpp>

#include <FishGUI/Window.hpp>
#include <FishGUI/Input.hpp>
#include <FishGUI/Draw.hpp>
#include <FishGUI/Shader.hpp>

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

#include <nanovg.h>


namespace FishGUI
{
	void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		//printf("[glfwKeyCallback] key: %d, scancode: %d, actions: %d, mods: %d\n", key, scancode, action, mods);
		auto win = WindowManager::GetInstance().FindWindow(window);
		auto& input = win->GetInput();
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		
		KeyEvent::Type type = KeyEvent::Type::KeyPress;	// GLFW_PRESS, GLFW_REPEAT
		if (action == GLFW_RELEASE)
			type = KeyEvent::Type::KeyRelease;
		//auto type = action==GLFW_PRESS ? KeyEvent::Type::KeyPress : KeyEvent::Type::KeyRelease;
		auto e = new KeyEvent(type, key, mods);
		input.m_keyEvents.push_back(e);
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
		//printf("[glfwMouseButtonCallback] button: %d, actions: %d, mods: %d\n", button, action, mods);
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
	}
	
	void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
//		printf("xoffset: %le yoffset: %le\n", xoffset, yoffset);
		auto& input = WindowManager::GetInstance().FindWindow(window)->GetInput();
		input.m_scrolling = true;
		input.m_scroll.x = static_cast<float>(xoffset*6);
		input.m_scroll.y = static_cast<float>(yoffset*6);
	}
	
	void glfwWindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto win = WindowManager::GetInstance().FindWindow(window);
		win->OnResize(width, height);
	}

	void glfwBindWindowCallbacks(GLFWwindow* window)
	{
		glfwSetKeyCallback(window, glfwKeyCallback);
		glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
		glfwSetScrollCallback(window, glfwScrollCallback);
		glfwSetCharCallback(window, glfwCharCallback);
		glfwSetWindowSizeCallback(window, glfwWindowResizeCallback);
//		glfwSetglfwWindowFocusCallback(window, glfwWindowFocusCallback);
	}
	
	Window::Window(FishGUIContext* context, const char* title,  int width, int height)
		: m_name(title), m_size{ width, height }, m_context(context)
	{
		m_input.m_window = this;
		m_minSize.width = 200;
		m_minSize.height = 200;
		
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
		glfwGetFramebufferSize(m_glfwWindow, &m_framebufferSize.width, &m_framebufferSize.height);
		
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
	
	
	//void Window::SetDecorated(bool decorated)
	//{
	//	assert(m_glfwWindow != nullptr);
	//	int v = decorated ? GLFW_TRUE : GLFW_FALSE;
	//	glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, v);
	//}
	
	//void Window::PreDraw()
	//{

	//}

	void Window::AfterDraw()
	{
		glfwMakeContextCurrent(m_glfwWindow);
		glfwSwapBuffers(m_glfwWindow);
	}


	void Window::BeforeFrame()
	{
		Context::GetInstance().BindWindow(this);
		//glfwMakeContextCurrent(m_glfwWindow);
		
		m_isFocused = (glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED) == 1);
		
		m_widgets.clear();
		
		glViewport(0, 0, m_framebufferSize.width, m_framebufferSize.height);
		float bck = 162 / 255.0f;
		glClearColor(bck, bck, bck, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		float ratio = float(m_framebufferSize.width) / m_size.width;
		
		PreDraw();
//		glfwSwapBuffers(m_glfwWindow);
		
		glViewport(0, 0, m_framebufferSize.width, m_framebufferSize.height);
		glClearColor(bck, bck, bck, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		Context::GetInstance().m_drawContext->vg = this->GetNVGContext();
		Context::GetInstance().m_input = &m_input;
		nvgBeginFrame(GetNVGContext(), m_size.width, m_size.height, ratio);
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
		//glfwSwapBuffers(m_glfwWindow);
	}
	
	void Window::Draw()
	{
		int iconified = glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED);
		if (iconified)
			return;
		BeforeFrame();
		if (m_layout != nullptr)
		{
			Rect rect = {0, 0, m_size.width, m_size.height};
			m_layout->PerformLayout(rect);
		}
		
		AfterFrame();
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


	void Window::OnResize(int w, int h)
	{
		if (m_size.width == w && m_size.height == h)
			return;
		m_size.width = w;
		m_size.height = h;
		glfwGetFramebufferSize(m_glfwWindow, &m_framebufferSize.width, &m_framebufferSize.height);
	}

	void Window::OnMouseEvent(MouseEvent* e)
	{
		Widget* newFocused = nullptr;
		if (e->type() == MouseEvent::Type::MouseButtonPress)
		{
			//if (e->button() == MouseButton::Left)
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


	static const char* screenVS = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  
)";
	static const char* screenFS = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
} 
)";
	static Shader* screenShader;


	Dialog::Dialog(FishGUIContext* context, const char* title, int width, int height)
		: Window(context, title, width, height)
	{
		BindMainContext();
		if (screenShader == nullptr)
		{
			screenShader = new Shader(screenVS, screenFS);
		}

		int fbw = m_framebufferSize.width;
		int fbh = m_framebufferSize.height;

		glGenFramebuffers(1, &m_framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		glGenTextures(1, &m_colorbuffer);
		glBindTexture(GL_TEXTURE_2D, m_colorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbw, fbh, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorbuffer, 0);

		glGenRenderbuffers(1, &m_renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fbw, fbh);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			abort();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glCheckError();

		glfwMakeContextCurrent(m_glfwWindow);

		static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
								 // positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		// screen quad VAO
		//unsigned int quadVAO, quadVBO;
		auto& quadVAO = m_quadVAO;
		auto& quadVBO = m_quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		//glCheckError();
	}

	Dialog::~Dialog()
	{
		BindMainContext();
		glDeleteFramebuffers(1, &m_framebuffer);
		glDeleteTextures(1, &m_colorbuffer);

		glfwMakeContextCurrent(m_glfwWindow);
		glDeleteVertexArrays(1, &m_quadVAO);
		glDeleteBuffers(1, &m_quadVBO);
		//glCheckError();
	}


	void Dialog::OnResize(int w, int h)
	{
		Window::OnResize(w, h);
		int fbw = m_framebufferSize.width;
		int fbh = m_framebufferSize.height;
		
		glBindTexture(GL_TEXTURE_2D, m_colorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbw, fbh, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fbw, fbh);
	}

	void Dialog::BindMainContext()
	{
		auto w = WindowManager::GetInstance().GetMainWindow()->GetGLFWWindow();
		glfwMakeContextCurrent(w);
	}

	void Dialog::Draw()
	{
		int iconified = glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED);
		if (iconified)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		BeforeFrame();

		if (m_layout != nullptr)
		{
			Rect rect = { 0, 0, m_size.width, m_size.height };
			m_layout->PerformLayout(rect);
		}

		//AfterFrame();
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Dialog::AfterDraw()
	{
		glfwMakeContextCurrent(m_glfwWindow);
		glViewport(0, 0, m_framebufferSize.width, m_framebufferSize.height);
		glClearColor(1, 0, 0, 1);	// error color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		screenShader->Use();
		screenShader->SetInt("screenTexture", 0);
		glBindVertexArray(m_quadVAO);
		glBindTexture(GL_TEXTURE_2D, m_colorbuffer);	// use the color attachment texture as the texture of the quad plane
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(m_glfwWindow);
		//glCheckError();
	}

}
