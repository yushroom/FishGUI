#ifdef _WIN32
//#define GLEW_STATIC
#include <GL/glew.h>
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#define GLFW_INCLUDE_GLCOREARB
#else
//#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include "Draw.hpp"
#define NANOVG_GL3 1
#include "nanovg_gl.h"
#include "Theme.hpp"

#include "FishGUI.hpp"
#include "Input.hpp"
#include "Window.hpp"
#include "Widget.hpp"

#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

void _checkOpenGLError(const char *file, int line)
{
	GLenum err = glGetError();

	while (err != GL_NO_ERROR)
	{
		const char* error;

		switch (err)
		{
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			default:                        error = "UNKNOWN_ERROR";
		}

		//		LogError(Format("GL_%1% - %2%:%3%", error, file, line));
		printf("[%d] GL_%s\n", line, error);
		err = glGetError();
	}
}

#define glCheckError() _checkOpenGLError(__FILE__,__LINE__)

extern Theme theme;

namespace FishGUI
{
	int DrawTabHeader(NVGcontext* ctx, float x, float y, float w, float h, const std::vector<std::string>& tabNames, int activeHeaderId)
	{
		int newActivateId = activeHeaderId;
		for (int i = 0; i < tabNames.size(); ++i)
		{
			auto name = tabNames[i].c_str();
			Rect r;
			r.x = x + theme.mTabHeaderCellWidth*i;
			r.y = y;
			r.width = theme.mTabHeaderCellWidth;
			r.height = theme.mWindowHeaderHeight;
			DrawTabButton(ctx, name, r.x, r.y, r.width, r.height, activeHeaderId == i);
			auto& input = Input::GetInstance();
			if (input.GetMouseButtonDown(MouseButton::Left) && input.MouseInRect(r))
			{
				newActivateId = i;
			}
			if (input.GetMouseButton(MouseButton::Left) && input.MouseInRect(r))
			{
				std::cout << "tab " << name << " dragged.\n";
			}
		}
		return newActivateId;
	}


	void ToolBar::Draw()
	{

	}

	void StatusBar::Draw()
	{
		auto ctx = m_context->m_nvgContext;
		nvgFontSize(ctx, 16.0f);
		nvgFontFace(ctx, "sans");
		nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
		nvgFillColor(ctx, theme.mTextColor);
		auto& input = Input::GetInstance();
		std::ostringstream sout;
		sout << "pos(" << input.m_mousePosition.x << ", " << input.m_mousePosition.y << ")";
		nvgText(ctx, m_rect.x + 5, m_rect.y + m_rect.height, sout.str().c_str(), nullptr);
	}

	void TabWidget::Draw()
	{
		auto ctx = m_context->m_nvgContext;
		auto r = m_rect;

		nvgSave(ctx);
		nvgScissor(ctx, r.x, r.y, r.width, r.height);

		if (!m_children.empty())
		{
			std::vector<std::string> tabNames(m_children.size());
			for (int i = 0; i < m_children.size(); ++i)
			{
				tabNames[i] = m_children[i]->GetName();
			}
			m_activeTabId = DrawTabHeader(ctx, r.x, r.y, r.width, r.height, tabNames, m_activeTabId);
			r.y += theme.mWindowHeaderHeight;
			r.height -= theme.mWindowHeaderHeight;
		}

		// content background
		nvgFillColor(ctx, theme.mTabContentBackgroundColor);
		nvgRect(ctx, r.x, r.y, r.width, r.height);
		nvgFill(ctx);

//		Widget::Draw();
		auto content = m_children[m_activeTabId];
		content->SetRect(r.x, r.y, r.width, r.height);
		content->Draw();
		
		nvgRestore(ctx);
	}
	
	void Window::BeforeFrame()
	{
		glfwMakeContextCurrent(m_glfwWindow);
		
		int width = m_size.width;
		int height = m_size.height;
		int fbWidth = m_size.width;
		int fbHeight = m_size.height;
		glfwGetWindowSize(m_glfwWindow, &width, &height);
		glfwGetFramebufferSize(m_glfwWindow, &fbWidth, &fbHeight);
		
		m_size.width = width;
		m_size.height = height;
		m_rect.width = width;
		m_rect.height = height;
		
//		GLNVGcontext * gl = (GLNVGcontext *)nvgInternalParams(m_context->m_nvgContext)->userPtr;
//		if (m_buffers == nullptr)
//		{
//			m_buffers = glnvg__createVertexBuffers(gl);
//		}
//		gl->buffers = m_buffers;
//		Rect remains{ 0, 0, m_size.width, m_size.height };
		
		glViewport(0, 0, fbWidth, fbHeight);
		float bck = 162 / 255.0f;
		glClearColor(bck, bck, bck, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		float ratio = float(fbWidth) / width;
		nvgBeginFrame(m_context->m_nvgContext, width, height, ratio);
	}
	
	void Window::AfterFrame()
	{
		nvgEndFrame(m_context->m_nvgContext);
		glfwSwapBuffers(m_glfwWindow);
	}
	
	void Window::Draw()
	{
		int iconified = glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED);
		if (iconified)
			return;
		BeforeFrame();
		Widget::Draw();
		AfterFrame();
	}

	void MainWindow::Draw()
	{
		int iconified = glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED);
		if (iconified)
			return;
		
		BeforeFrame();

		m_toolBar.SetRect(0, 0, m_rect.width, m_toolBar.GetHeight());
		m_toolBar.Draw();
		m_statusBar.SetRect(0, m_rect.height - m_statusBar.GetHeight(), m_rect.width, m_statusBar.GetHeight());
		m_statusBar.Draw();

		m_rect.height -= m_toolBar.GetHeight() + m_statusBar.GetHeight();
		m_rect.y += m_toolBar.GetHeight();
		Widget::Draw();
		m_rect.height += m_toolBar.GetHeight() + m_statusBar.GetHeight();
		m_rect.y -= m_toolBar.GetHeight();

		AfterFrame();
	}
	
	static void GlfwErrorCallback(int error, const char* description)
	{
		fputs(description, stderr);
	}
	
	Window* NewWindow(const char* title)
	{
		auto context = WindowManager::GetInstance().GetMainWindow()->GetContext();
		auto window = new Window(context, title, 400, 400);
		WindowManager::GetInstance().m_windows.push_back(window);
		return window;
	}

	void Init()
	{
		static FishGUIContext context;
		glfwSetErrorCallback(GlfwErrorCallback);
		if (!glfwInit())
		{
			exit(EXIT_FAILURE);
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
//		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
		
		auto window = new MainWindow(&context, "Fish GUI", 800, 600);
		WindowManager::GetInstance().m_windows.push_back(window);
		glfwMakeContextCurrent(window->GetGLFWWindow());
		glfwSwapInterval(1);
		
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
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

		context.m_nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

		nvgCreateFont(context.m_nvgContext, "icons", "../entypo.ttf");
		nvgCreateFont(context.m_nvgContext, "sans", "../Roboto-Regular.ttf");
		nvgCreateFont(context.m_nvgContext, "sans-bold", "../Roboto-Bold.ttf");
		nvgCreateFont(context.m_nvgContext, "emoji", "../NotoEmoji-Regular.ttf");

		Cursor::GetInstance().Init();
	}

	void Cursor::Draw()
	{
		if (m_cursorType != m_cursorTypeLastFame) {
			glfwSetCursor(WindowManager::GetInstance().GetMainWindow()->GetGLFWWindow(), m_glfwCursors[static_cast<int>(m_cursorType)]);
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
		auto mainWindow = WindowManager::GetInstance().GetMainWindow();
		
		double lastTime = glfwGetTime();
		
		while (!glfwWindowShouldClose(mainWindow->GetGLFWWindow()))
		{
			// get mouse position
			auto& input = Input::GetInstance();
			double mx, my;
			glfwGetCursorPos(mainWindow->GetGLFWWindow(), &mx, &my);
			input.m_mousePosition.x = int(mx);
			input.m_mousePosition.y = int(my);
			
			BeforeFrame();
			auto& windows = WindowManager::GetInstance().GetWindows();
			for (auto w : windows)
			{
				w->Draw();
			}
			AfterFrame();
			
			input.Update();
			glfwPollEvents();
			
			Window* remove = nullptr;
			for (auto win : windows)
			{
				if (win == mainWindow)
					continue;
				auto w = win->GetGLFWWindow();
				if (glfwWindowShouldClose(w))
				{
					remove = win;
				}
			}
			if (remove != nullptr)
			{
				windows.remove(remove);
				delete remove;
			}
			
			double now = glfwGetTime();
			double interval = now - lastTime;
			constexpr int target_fps = 30;
			constexpr double target_interval = 1.0 / target_fps;
			if (interval < target_interval)
			{
//				std::cout << "time: " << interval << std::endl;
				std::this_thread::sleep_for((target_interval-interval)*1000ms);
			}
			lastTime = now;
			glCheckError();
		}
		
		glfwDestroyWindow(mainWindow->GetGLFWWindow());
		glfwTerminate();
	}
	
	float Clamp(float v, float minv, float maxv)
	{
		if (v < minv)
			return minv;
		if (v > maxv)
			return maxv;
		return v;
	}
	
	// horizontally divide rect into 2 rects, left & right
	void HSplitRect2(const Rect& r, Rect& r1, Rect& r2, float size1, float size2, int interval)
	{
		r1 = r2 = r;
		int w1 = static_cast<int>((r.width - interval) / (size1+size2) * size1);
		int w2 = r.width - interval - w1;
		r1.width = w1;
		r2.width = w2;
		r2.x = r1.x + w1 + interval;
	}
	
	// horizontally divide rect into 3 rects
	void HSplitRect3(const Rect& r, Rect& r1, Rect& r2, Rect& r3, float size1, float size2, float size3, int interval)
	{
		r1 = r2 = r3 = r;
		float s = (r.width - interval*2) / (size1+size2+size3);
		int w1 = static_cast<int>(s * size1);
		int w2 = static_cast<int>(s * size2);
		int w3 = r.width - interval*2 - w1 - w2;
		r1.width = w1;
		r2.width = w2;
		r3.width = w3;
		r2.x = r1.x + w1 + interval;
		r3.x = r2.x + w2 + interval;
	}
	
	// slower than std:to_string but the result is more elegant
	std::string ToString(float x)
	{
		std::ostringstream sout;
		sout << x;
		return sout.str();
	}
	
	struct IMGUIContext
	{
		IMWidget* 		widget = nullptr;
		Rect 			rect;
		Vector2i 		pos;
		float 			yStart = 0;
//		int				totalHeight = 0;
		static constexpr int cellHeight = 16;
		static constexpr int xmargin = 2;
		static constexpr int ymargin = 2;
		static constexpr int Indent = 10;
		
		// scroll bar status depends on last farme status
		bool showScrollBar = false;
		bool needScrollBar = false;
		static constexpr int scrollBarWidth = 10;
		
		int indent = 0;
		
		void BeginFrame()
		{
			rect = widget->GetRect();
			pos.x = rect.x;
			pos.y = rect.y + yStart;
			showScrollBar = needScrollBar;	// set by last frame
			needScrollBar = false;			// clear for this frame
//			totalHeight = 0;
		}
		
		void EndFrame()
		{
			const int total_height = pos.y - (yStart+rect.y);
			if (total_height > rect.y + rect.height)
			{
				needScrollBar = true;
			}
//			Label("yStart="+ToString(yStart)+" pos.y="+std::to_string(pos.y) + " rect.bottom="+std::to_string(rect.y+rect.height));
//			Label("yStart="+ToString(yStart)+" pos.y="+std::to_string(pos.y) + " totalHeight="+std::to_string(totalHeight));
			if (showScrollBar)
			{
				auto ctx = GetContext();
				int x = rect.x+rect.width-scrollBarWidth;
				int y = rect.y;
				int w = scrollBarWidth;
				int h = rect.height;
				DrawRect(ctx, x, y, w, h, theme.mScrollBarBackColor);
				
				const int total_height = pos.y - (yStart+rect.y);
				float p = float(rect.height) / total_height;
				constexpr int pad = 1;
				x = rect.x+rect.width-scrollBarWidth+pad;
//				y = rect.y + float(rect.height)*(-yStart)/total_height;
				y = rect.y + (-yStart)*p;
				w = scrollBarWidth - pad*2;
				h = p * rect.height;
				int r = scrollBarWidth / 2 - pad;
				DrawRoundedRect(ctx, x, y, w, h, r, theme.mScrollBarColor);
				
				auto& input = Input::GetInstance();
				if (input.m_scrolling && input.MouseInRect(rect))
				{
					yStart += input.m_scroll.y*4;
					printf("%lf\n", yStart);
					yStart = Clamp(yStart, (rect.height - total_height), 0);
				}
			}
		}
		
		Rect NextCell(int height = cellHeight)
		{
			auto ret = rect;
			ret.x = pos.x + xmargin + indent;
			ret.y = pos.y + ymargin;
			ret.width -= xmargin*2 + indent + (showScrollBar ? scrollBarWidth : 0);
			ret.height = height;
			pos.y += ymargin + height;
//			const int total_height = pos.y - yStart;
//			if (total_height > rect.y + rect.height)
//			{
//				needScrollBar = true;
//			}
//			totalHeight += ymargin+height;
			return ret;
		}
		
		// label + ...
		void NextCell2(Rect& left, Rect& right, float leftLen = 1, float rightLen = 1, int height = cellHeight)
		{
			auto r = NextCell(cellHeight);
			HSplitRect2(r, left, right, leftLen, rightLen, xmargin);
		}
		
		void AddIndent(int indent = Indent)
		{
			this->indent += indent;
		}
		
		void DrawLine()
		{
			pos.y += ymargin;
			::DrawLine(GetContext(), pos.x, pos.y, pos.x+rect.width-(showScrollBar ? scrollBarWidth : 0), pos.y);
//			totalHeight += ymargin;
		}
		
		NVGcontext* GetContext()
		{
			return widget->GetContext()->m_nvgContext;
		}
	};
	
	static IMGUIContext* g_IMContext = nullptr;
	
	IMWidget::IMWidget(FishGUIContext* context, const char* name)
		: Widget(context, name)
	{
		m_imContext = new IMGUIContext;
		m_imContext->widget = this;
	}
	
	IMWidget::~IMWidget()
	{
		delete m_imContext;
	}
	
	void IMWidget::Draw()
	{
		auto& r = m_rect;
		auto ctx = m_context->m_nvgContext;
//		DrawRoundedRect(ctx, r.x, r.y, r.width, r.height, 10, nvgRGBA(0, 0, 0, 255));
		nvgIntersectScissor(ctx, r.x, r.y, r.width, r.height);
		g_IMContext = m_imContext;
		m_imContext->BeginFrame();
		if (m_renderFunction != nullptr)
			m_renderFunction();
		m_imContext->EndFrame();
		g_IMContext = nullptr;
		nvgResetScissor(ctx);
	}
	
	void Group(const std::string & name)
	{
		Label(name);
		g_IMContext->AddIndent(IMGUIContext::Indent);
	}
	
	void EndGroup()
	{
		g_IMContext->AddIndent(-IMGUIContext::Indent);
		g_IMContext->DrawLine();
	}
	
	bool Button(const std::string & text)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = g_IMContext->widget->GetContext()->m_nvgContext;
		auto r = g_IMContext->NextCell(20);
		DrawButton(ctx, 0, text.c_str(), r.x, r.y, r.width, r.height);
		return false;
	}
	
	void Label(const std::string & text)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = g_IMContext->GetContext();
		auto r = g_IMContext->NextCell();
		DrawLabel(ctx, text.c_str(), r.x, r.y, r.width, r.height);
	}

	void CheckBox(const std::string & label, bool& inoutValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = g_IMContext->GetContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2);
		DrawLabel(ctx, label.c_str(), r1.x, r1.y, r1.width, r1.height);
		DrawCheckBox(ctx, r2.x, r2.y, r2.width, r2.height);
	}
	
	void InputText(const std::string & label, std::string& inoutValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = g_IMContext->GetContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2);
		DrawLabel(ctx, label.c_str(), r1.x, r1.y, r1.width, r1.height);
		DrawEditBox(ctx, inoutValue.c_str(), r2.x, r2.y, r2.width, r2.height);
	}
	
	bool Slider(const std::string & label, float& inoutValue, float minValue, float maxValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = g_IMContext->GetContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2);
		float pos = (inoutValue - minValue) / (maxValue - minValue);
		DrawLabel(ctx, label.c_str(), r1.x, r1.y, r1.width, r1.height);
		DrawSlider(ctx, pos, r2.x, r2.y, r2.width, r2.height);
		return false;
	}
	
	void Float3(const std::string & label, float& x, float& y, float& z)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = g_IMContext->GetContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2, 1, 3);
		
		DrawLabel(ctx, label.c_str(), r1.x, r1.y, r1.width, r1.height);
		
		auto x_str = ToString(x);
		auto y_str = ToString(y);
		auto z_str = ToString(z);
		Rect r21, r22, r23;
		constexpr int pad = 2;
		HSplitRect3(r2, r21, r22, r23, 1, 1, 1, pad);
		constexpr int label_len = 11;
		constexpr int align = NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE;
		DrawLabel(ctx, "X", r21.x+pad, r21.y, label_len, r21.height, align);
		r21.x += label_len;
		DrawEditBox(ctx, x_str.c_str(), r21.x, r21.y, r21.width-label_len, r21.height);
		DrawLabel(ctx, "Y", r22.x+pad, r22.y, label_len, r22.height, align);
		r22.x += label_len;
		DrawEditBox(ctx, y_str.c_str(), r22.x, r22.y, r22.width-label_len, r22.height);
		DrawLabel(ctx, "Z", r23.x+pad, r23.y, label_len, r23.height, align);
		r23.x += label_len;
		DrawEditBox(ctx, z_str.c_str(), r23.x, r23.y, r23.width-label_len, r22.height);
	}
}
