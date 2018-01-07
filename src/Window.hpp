#pragma once
#include "FishGUI.hpp"

#include <cassert>
#include <list>

#include <GLFW/glfw3.h>
#include "Widget.hpp"

struct GLFWwindow;
struct GLNVGvertexBuffers;

namespace FishGUI
{
	class Window : public Widget
	{
	public:
		Window(FishGUIContext* context, const char* title, int width, int height);

		Window(Window&) = delete;
		Window& operator=(Window&) = delete;
		
		virtual ~Window();
		
		float PixelRatio() const { return float(m_frameBufferSize.width) / m_size.width; }
		
		GLFWwindow * GetGLFWWindow() { return m_glfwWindow; }
		FishGUIContext * GetContext() { return m_context; }
		
		void SetPosition(int x, int y)
		{
			assert(m_glfwWindow != nullptr);
			glfwSetWindowPos(m_glfwWindow, x, y);
			m_position.x = x;
			m_position.y = y;
		}
		
		void SetSize(int width, int height)
		{
			assert(m_glfwWindow != nullptr);
			glfwSetWindowSize(m_glfwWindow, width, height);
			m_size.width = width;
			m_size.height = height;
		}
		
		void SetTitle(const char* title)
		{
			assert(m_glfwWindow != nullptr);
			m_name = title;
			glfwSetWindowTitle(m_glfwWindow, title);
		}
		
		bool IsFocused() const { return m_isFocused; }
		
		void SetDecorated(bool decorated)
		{
			assert(m_glfwWindow != nullptr);
			int v = decorated ? GLFW_TRUE : GLFW_FALSE;
			glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, v);
		}
		
		void BeforeFrame();
		virtual void Draw() override;
		void AfterFrame();
		
		void OverlayDraw();
		
	protected:
		Size				m_size;
		Vector2i			m_position;
		Size 				m_frameBufferSize;
		GLFWwindow* 		m_glfwWindow;
		GLNVGvertexBuffers* m_buffers = nullptr;
		
		// a window which is not focused may be rendered as normal, but will not receive input events
		bool				m_isFocused = false;
		
		std::function<void(void)> m_overlayDrawFunction;
	};

	class MainWindow : public Window
	{
	public:
		MainWindow(FishGUIContext* context, const char* title, int width, int height);

		MainWindow(MainWindow&) = delete;
		MainWindow& operator=(MainWindow&) = delete;

		virtual void Draw() override;

	protected:
		ToolBar m_toolBar;
		StatusBar m_statusBar;
	};

	
	class WindowManager
	{
	public:
		static WindowManager& GetInstance()
		{
			static WindowManager mgr;
			return mgr;
		}

		WindowManager() = default;
		WindowManager(WindowManager&) = delete;
		WindowManager& operator=(WindowManager&) = delete;
		
		std::list<Window*>& GetWindows()
		{
			return m_windows;
		}
		
		Window* GetMainWindow()
		{
			if (m_windows.empty())
				return nullptr;
			return m_windows.front();
		}
		
		Window* GetFocusedWindow()
		{
			for (auto w : m_windows)
			{
				if (w->IsFocused())
					return w;
			}
			return nullptr;
		}
		
		void CloseWindow(Window* window)
		{
			m_windows.remove(window);
			delete window;
		}
		
		Window* FindWindow(GLFWwindow* glfwWindow)
		{
			for (auto w : m_windows)
			{
				if (w->GetGLFWWindow() == glfwWindow)
					return w;
			}
			return nullptr;
		}
		
		std::list<Window*> m_windows;
	};
}
