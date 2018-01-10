#pragma once
#include "FishGUI.hpp"

#include <cassert>
#include <list>
#include <map>
#include <functional>

#include "Widget.hpp"
#include "Input.hpp"

struct GLFWwindow;

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
		FishGUIContext* GetContext() { return m_context; }
		NVGcontext * GetNVGContext() { return m_context->m_nvgContext; }
		
		bool IsFocused() const { return m_isFocused; }
		
		void SetPosition(int x, int y);
		void SetSize(int width, int height);
		void SetTitle(const char* title);
		void SetDecorated(bool decorated);
		
		void BeforeFrame();
		virtual void Draw() override;
		void AfterFrame();
		
		void OverlayDraw();
		
		Input& GetInput()
		{
			return m_input;
		}
		
	protected:
		Size				m_size;
		Vector2i			m_position;
		Size 				m_frameBufferSize;
		GLFWwindow* 		m_glfwWindow;
		Input				m_input;
		FishGUIContext*		m_context;
//		NVGcontext*			m_nvgContext;
		
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
		
		void SetToolBar(ToolBar* toolBar)
		{
			m_toolBar = toolBar;
		}
		
		void SetStatusBar(StatusBar* statusBar)
		{
			m_statusBar = statusBar;
		}
		
		ToolBar* GetToolBar()
		{
			return m_toolBar;
		}
		
		StatusBar* GetStatusBar()
		{
			return m_statusBar;
		}

	protected:
		ToolBar* 	m_toolBar 	= nullptr;
		StatusBar* 	m_statusBar = nullptr;
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
