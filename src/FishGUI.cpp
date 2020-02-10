#include <FishGUI/GLEnvironment.hpp>
#if FISHENGINE_PLATFORM_APPLE
#	define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

#include <FishGUI/Draw.hpp>
#define NANOVG_GL3 1
#include <nanovg/nanovg_gl.h>
#include <FishGUI/Theme.hpp>

#include <FishGUI/FishGUI.hpp>
//#include <FishGUI/Input.hpp>
#include <FishGUI/Window.hpp>
//#include <FishGUI/Widget.hpp>
#include <FishGUI/Utils.hpp>

// #include <sstream>
//#include <vector>
#include <thread>
#include <chrono>
//#include <iostream>

using namespace std::chrono_literals;

namespace FishGUI
{
	Context::Context()
	{
		m_drawContext = new DrawContext();
	}

	Context::~Context()
	{
		delete m_drawContext;
	}

	void Context::BindWindow(Window* window)
	{
		m_window = window;
	}

	void Context::BindWidget(Widget* widget)
	{
		m_widget = widget;
		m_window->m_widgets.push_back(m_widget);
	}

	void Context::UnbindWindow()
	{
		m_window = nullptr;
	}

	void Context::UnbindWidget()
	{
		m_widget = nullptr;
	}

	inline Context* CurrentContext()
	{
		return &Context::GetInstance();
	}

	float Context::GetPixelRatioOfCurrentWindow()
	{
		if (m_window != nullptr)
			return m_window->PixelRatio();
		return 0.0f;
	}


	inline Input* CurrentInput()
	{
		return Context::GetInstance().m_input;
	}

	inline Theme* CurrentTheme()
	{
		return Context::GetInstance().m_drawContext->theme;
	}

	inline NVGcontext* GetNVGContext()
	{
		return Context::GetInstance().m_drawContext->vg;
	}

	inline DrawContext* GetDrawContext()
	{
		return Context::GetInstance().m_drawContext;
	}


	int DrawTabHeader(DrawContext* context, int x, int y, int w, int h, const std::vector<std::string>& tabNames, int activeHeaderId)
	{
		int newActivateId = activeHeaderId;
		for (int i = 0; i < tabNames.size(); ++i)
		{
			auto name = tabNames[i].c_str();
			Rect r;
			r.x = x + context->theme->tabHeaderCellWidth*i;
			r.y = y;
			r.width = context->theme->tabHeaderCellWidth;
			r.height = context->theme->windowHeaderHeight;
			DrawTabButton(context, name, r.x, r.y, r.width, r.height, activeHeaderId == i);
			auto input = Input::GetCurrent();
			if (input->GetMouseButtonDown(MouseButton::Left) && input->MouseInRect(r))
			{
				newActivateId = i;
			}
//			if (input->GetMouseButton(MouseButton::Left) && input->MouseInRect(r))
//			{
//				std::cout << "tab " << name << " dragged.\n";
//			}
		}
		return newActivateId;
	}

	void Widget::BindAndDraw()
	{
		// bind
		Context::GetInstance().BindWidget(this);
		if (m_theme != nullptr)
			GetDrawContext()->theme = m_theme;
		else
			GetDrawContext()->theme = Theme::GetDefaultTheme();

		Draw();

		// unbind
		GetDrawContext()->theme = nullptr;
		Context::GetInstance().UnbindWidget();

		// clear events
		m_keyEvent = nullptr;
		m_mouseEvent = nullptr;
	}


	void TabWidget::Draw()
	{
		auto vg = GetNVGContext();
		auto theme = CurrentTheme();
		auto r = m_rect;

		nvgSave(vg);
		nvgScissor(vg, (float)r.x, (float)r.y, (float)r.width, (float)r.height);

		if (!m_children.empty())
		{
			std::vector<std::string> tabNames(m_children.size());
			for (int i = 0; i < m_children.size(); ++i)
			{
				tabNames[i] = m_children[i]->GetName();
			}
			m_activeTabId = DrawTabHeader(GetDrawContext(), r.x, r.y, r.width, r.height, tabNames, m_activeTabId);
			r.y += theme->windowHeaderHeight;
			r.height -= theme->windowHeaderHeight;
		}

		// content background
		nvgFillColor(vg, theme->tabContentBackgroundColor);
		nvgRect(vg, (float)r.x, (float)r.y, (float)r.width, (float)r.height);
		nvgFill(vg);

//		nvgBeginPath(vg);
//		nvgRect(vg, r.x + 0.5f, r.y + 0.5f, r.width - 1, r.height - 1);
//		nvgStrokeColor(vg, nvgRGB(0, 0, 0));
//		nvgStroke(vg);

//		Widget::Draw();
		if (!m_children.empty())
		{
			auto content = m_children[m_activeTabId];
			content->SetRect(r.x, r.y, r.width, r.height);
			content->BindAndDraw();
		}

		nvgRestore(vg);
	}



	static void GlfwErrorCallback(int error, const char* description)
	{
		fputs(description, stderr);
	}



	void CreateFont(NVGcontext* ctx, const char* name, const std::string& path)
	{
		auto ret = nvgCreateFont(ctx, name, path.c_str());
		if (ret == -1)
		{
			printf("[ERROR] font not found: %s\n", path.c_str());
		}
	}

	static FishGUIContext context;

	Window* NewWindow(const char* title, int w, int h)
	{
		auto dialog = new Window(&context, title, w, h);
//		WindowManager::GetInstance().m_windows.push_back(dialog);
		return dialog;
	}


	void Init()
	{
//		static FishGUIContext context;
		glfwSetErrorCallback(GlfwErrorCallback);
		if (!glfwInit())
		{
			exit(EXIT_FAILURE);
		}
#if __linux__
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 4);
//		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		auto contextWindow = glfwCreateWindow(1, 1, "context", nullptr, nullptr);
		glfwMakeContextCurrent(contextWindow);
		glfwSwapInterval(1);
		context.m_contextWindow = contextWindow;
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

#if ! __APPLE__
		// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			exit(EXIT_FAILURE);
		}
		//fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif
		glEnable(GL_MULTISAMPLE);

		context.m_nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

		auto resourcesRoot = ApplicationFilePath();
		auto fontsRoot = resourcesRoot + "/fonts/";
		CreateFont(context.m_nvgContext, "icons", 		fontsRoot+"entypo.ttf");
		CreateFont(context.m_nvgContext, "sans", 		fontsRoot+"Roboto-Regular.ttf");
		CreateFont(context.m_nvgContext, "sans-bold", 	fontsRoot+"Roboto-Bold.ttf");
		CreateFont(context.m_nvgContext, "emoji", 		fontsRoot+"NotoEmoji-Regular.ttf");
		CreateFont(context.m_nvgContext, "ui", 			fontsRoot+"icomoon.ttf");

		Cursor::GetInstance().Init();
	}

	void Cursor::Draw()
	{
		if (m_cursorType != m_cursorTypeLastFame) {
			glfwSetCursor(context.m_contextWindow, m_glfwCursors[static_cast<int>(m_cursorType)]);
			m_cursorTypeLastFame = m_cursorType;
		}
	}

	void BeforeFrame()
	{
		Cursor::GetInstance().SetCursorType(CursorType::Arrow);
	}

	void AfterFrame()
	{
		Cursor::GetInstance().Draw();
	}

	void Run()
	{
//		auto mainWindow = WindowManager::GetInstance().GetMainWindow();

		double lastTime = glfwGetTime();

		while (true)
		{
			// get mouse position
			double mx, my;
			auto& windows = WindowManager::GetInstance().GetWindows();
			for (auto w : windows)
			{
				glfwMakeContextCurrent(context.m_contextWindow);
				auto& input = w->GetInput();
				glfwGetCursorPos(w->GetGLFWWindow(), &mx, &my);
				input.m_mousePosition.x = int(mx);
				input.m_mousePosition.y = int(my);

				w->BeforeFrame();
			}

			BeforeFrame();
			for (auto w : windows)
			{
				glfwMakeContextCurrent(context.m_contextWindow);
				w->BindAndDraw();
			}
			AfterFrame();

//			glfwSwapBuffers(context.m_contextWindow);
			glFlush();

			for (auto w : windows)
			{
				w->AfterFrame();
				auto& input = w->GetInput();
				input.Update();
			}
			glfwPollEvents();

			Window* remove = nullptr;
			for (auto win : windows)
			{
				auto w = win->GetGLFWWindow();
				if (glfwWindowShouldClose(w))
				{
					remove = win;
				}
			}
			if (remove != nullptr)
			{
//				glfwFocusWindow(mainWindow->GetGLFWWindow());
				windows.remove(remove);
				delete remove;
			}

			double now = glfwGetTime();
			double interval = now - lastTime;
			//printf("time: %lf ms\n", interval*1000);
			constexpr int target_fps = 30;
			constexpr double target_interval = 1.0 / target_fps;
			if (interval < target_interval)
			{
				std::this_thread::sleep_for((target_interval-interval)*1000ms);
			}
			lastTime = now;
			//glCheckError();

			if (windows.empty())
				break;
		}

		WindowManager::GetInstance().Clear();

		glfwDestroyWindow(context.m_contextWindow);
		glfwTerminate();
	}
}
