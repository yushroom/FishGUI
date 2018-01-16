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
	class Window
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
		virtual void Draw();
		void AfterFrame();
		
		void OnMouseEvent(MouseEvent* e);
		void OnKeyEvent(KeyEvent* e);
		
		void PreDraw()
		{
			if (m_preDrawFunction != nullptr)
				m_preDrawFunction();
		}
		
		void OverlayDraw()
		{
			if (m_overlayDrawFunction != nullptr)
				m_overlayDrawFunction();
		}
		
		Input& GetInput()
		{
			return m_input;
		}
		
		void SetLayout(Layout* layout)
		{
			m_layout = layout;
		}
		
		typedef std::function<void(void)> PreDrawFunction;
		void SetPreDraw(PreDrawFunction func)
		{
			m_preDrawFunction = func;
		}
		
		typedef std::function<void(void)> OverlayDrawFunction;
		void SetOverlayDraw(OverlayDrawFunction func)
		{
			m_overlayDrawFunction = func;
		}
		
		const std::vector<Widget*>& GetAllWidgets() const
		{
			return m_widgets;
		}
		
	protected:
		friend struct Context;
		std::string			m_name;
		Size				m_size;
		Size				m_minSize = {16, 16};
		Size				m_maxSize = {4096, 4096};
		Vector2i			m_position;
		Size 				m_frameBufferSize;
		GLFWwindow* 		m_glfwWindow;
		Input				m_input;
		FishGUIContext*		m_context;
		Layout*				m_layout = nullptr;
//		NVGcontext*			m_nvgContext;
		
		// visiable widgets in this frame
		std::vector<Widget*> m_widgets;
		Widget*				m_focusedWidget = nullptr;
		
		// a unfocused window may be rendered as normal, but will not receive input events(except for scroll event)
		bool				m_isFocused = false;
		
		std::function<void(void)> m_overlayDrawFunction;
		std::function<void(void)> m_preDrawFunction;
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
