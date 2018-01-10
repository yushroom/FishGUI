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

namespace FishGUI
{
	struct Context
	{
		DrawContext drawContext;
		Input*		input = nullptr;
		
		static Context& GetInstance()
		{
			static Context instance;
			return instance;
		}
		
	private:
		Context() = default;
	};
	
	static NVGcontext* GetNVGContext()
	{
		return Context::GetInstance().drawContext.vg;
	}
	
	static DrawContext* GetDrawContext()
	{
		return &Context::GetInstance().drawContext;
	}
	
	
	int DrawTabHeader(DrawContext* context, float x, float y, float w, float h, const std::vector<std::string>& tabNames, int activeHeaderId)
	{
		int newActivateId = activeHeaderId;
		for (int i = 0; i < tabNames.size(); ++i)
		{
			auto name = tabNames[i].c_str();
			Rect r;
			r.x = x + context->theme->mTabHeaderCellWidth*i;
			r.y = y;
			r.width = context->theme->mTabHeaderCellWidth;
			r.height = context->theme->mWindowHeaderHeight;
			DrawTabButton(context, name, r.x, r.y, r.width, r.height, activeHeaderId == i);
			auto input = Input::GetCurrent();
			if (input->GetMouseButtonDown(MouseButton::Left) && input->MouseInRect(r))
			{
				newActivateId = i;
			}
			if (input->GetMouseButton(MouseButton::Left) && input->MouseInRect(r))
			{
				std::cout << "tab " << name << " dragged.\n";
			}
		}
		return newActivateId;
	}

	void Widget::Draw2()
	{
		if (m_theme != nullptr)
			GetDrawContext()->theme = m_theme;
		else
			GetDrawContext()->theme = Theme::GetDefaultTheme();
		Draw();
		GetDrawContext()->theme = nullptr;
	}

	void StatusBar::Draw()
	{
//		auto ctx = m_context->m_nvgContext;

		auto input = Input::GetCurrent();
		std::ostringstream sout;
		sout << "pos(" << input->m_mousePosition.x << ", " << input->m_mousePosition.y << ")";
//		nvgFontSize(ctx, 16.0f);
//		nvgFontFace(ctx, "sans");
//		nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
//		nvgFillColor(ctx, theme.mTextColor);
//		nvgText(ctx, m_rect.x + 5, m_rect.y + m_rect.height, sout.str().c_str(), nullptr);
		auto r = m_rect;
		r.x += 5;
		Label(sout.str(), r);
	}

	void TabWidget::Draw()
	{
//		auto ctx = m_context->m_nvgContext;
		auto ctx = &Context::GetInstance().drawContext;
		auto vg = Context::GetInstance().drawContext.vg;
		auto theme = *Context::GetInstance().drawContext.theme;
		auto r = m_rect;

		nvgSave(vg);
		nvgScissor(vg, r.x, r.y, r.width, r.height);

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
		nvgFillColor(vg, theme.mTabContentBackgroundColor);
		nvgRect(vg, r.x, r.y, r.width, r.height);
		nvgFill(vg);

//		Widget::Draw();
		auto content = m_children[m_activeTabId];
		content->SetRect(r.x, r.y, r.width, r.height);
		content->Draw2();
		
		nvgRestore(vg);
	}
	
	void Window::BeforeFrame()
	{
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
		m_rect.width = width;
		m_rect.height = height;
		
		glViewport(0, 0, fbWidth, fbHeight);
		float bck = 162 / 255.0f;
		glClearColor(bck, bck, bck, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		Context::GetInstance().drawContext.vg = GetNVGContext();
		Context::GetInstance().input = &m_input;
		
		float ratio = float(fbWidth) / width;
		nvgBeginFrame(GetNVGContext(), width, height, ratio);
	}
	
	void Window::AfterFrame()
	{
		nvgEndFrame(GetNVGContext());
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

		if (m_toolBar != nullptr)
		{
			m_toolBar->SetRect(0, 0, m_rect.width, m_toolBar->GetHeight());
			m_toolBar->Draw2();
		}
		
		if (m_statusBar != nullptr)
		{
			m_statusBar->SetRect(0, m_rect.height - m_statusBar->GetHeight(), m_rect.width, m_statusBar->GetHeight());
			m_statusBar->Draw2();
		}
		if (m_toolBar != nullptr)
		{
			m_rect.height -= m_toolBar->GetHeight();
			m_rect.y += m_toolBar->GetHeight();
		}
		if (m_statusBar != nullptr)
		{
			m_rect.height -= m_statusBar->GetHeight();
		}
		
		Widget::Draw();
		
		if (m_toolBar != nullptr)
		{
			m_rect.height += m_toolBar->GetHeight();
			m_rect.y -= m_toolBar->GetHeight();
		}
		if (m_statusBar != nullptr)
		{
			m_rect.height += m_statusBar->GetHeight();
		}

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
		
		auto window = new MainWindow(&context, "Fish GUI", 950, 600);
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
		assert( -1 != nvgCreateFont(context.m_nvgContext, "ui", "../icon_ui.ttf") );

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
			double mx, my;
			auto& windows = WindowManager::GetInstance().GetWindows();
			for (auto w : windows)
			{
				auto& input = w->GetInput();
				glfwGetCursorPos(w->GetGLFWWindow(), &mx, &my);
				input.m_mousePosition.x = int(mx);
				input.m_mousePosition.y = int(my);
			}
			
			BeforeFrame();
			for (auto w : windows)
			{
				auto& input = w->GetInput();
				Input::SetCurrent(&input);
				w->Draw();
				Input::SetCurrent(nullptr);
			}
			AfterFrame();
			
			for (auto w : windows)
			{
				auto& input = w->GetInput();
				input.Update();
			}
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
				glfwFocusWindow(mainWindow->GetGLFWWindow());
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
	//  if size1 > 0 and size2 > 0:	width of left = (r.width - interval) * size1 / (size1+size2))
	//  if size1 == -1, then right has fixed width size2
	void HSplitRect2(const Rect& r, Rect& r1, Rect& r2, float size1, float size2, int interval)
	{
		r1 = r2 = r;
		int w1, w2;
		if (size1 <= 0 && size2 <= 0)
			size1 = size2 = 1;
		if (size1 > 0 && size2 > 0)
		{
			w1 = static_cast<int>((r.width - interval) * size1 / (size1+size2));
			w2 = r.width - interval - w1;
		}
		else if (size1 < 0)
		{
			w2 = size2;
			w1 = r.width - interval - w2;
		}
		else	// size2 < 0
		{
			w1 = size1;
			w2 = r.width - interval - w1;
		}
		
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
		Rect 			rect = {0, 0, 1, 1};
		Vector2i 		pos;			// start position of the next cell
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
		
		void Reset()
		{
			yStart = 0;
			pos.x = rect.x;
			pos.y = rect.y;
			indent = 0;
		}
		
		void BeginFrame()
		{
//			const int total_height = pos.y - (yStart+rect.y);
//			yStart = Clamp(yStart, (rect.height - total_height), 0);
			
			indent = 0;
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
			if (total_height > rect.height)
			{
				needScrollBar = true;
			}
//			Label("yStart="+ToString(yStart)+" pos.y="+std::to_string(pos.y) + " rect.bottom="+std::to_string(rect.y+rect.height));
//			Label("yStart="+ToString(yStart)+" pos.y="+std::to_string(pos.y) + " totalHeight="+std::to_string(totalHeight));
			auto ctx = GetDrawContext();
			auto theme = Context::GetInstance().drawContext.theme;
			if (showScrollBar)
			{
				int x = rect.x+rect.width-scrollBarWidth;
				int y = rect.y;
				int w = scrollBarWidth;
				int h = rect.height;
				DrawRect(ctx, x, y, w, h, theme->mScrollBarBackColor);
				
				const int total_height = pos.y - (yStart+rect.y);
				float p = float(rect.height) / total_height;
				constexpr int pad = 1;
				x = rect.x+rect.width-scrollBarWidth+pad;
//				y = rect.y + float(rect.height)*(-yStart)/total_height;
				y = rect.y + (-yStart)*p;
				w = scrollBarWidth - pad*2;
				h = p * rect.height;
				int r = scrollBarWidth / 2 - pad;
				DrawRoundedRect(ctx, x, y, w, h, r, theme->mScrollBarColor);
				
				auto input = Input::GetCurrent();
				if (input->m_scrolling && input->MouseInRect(rect))
				{
					yStart += input->m_scroll.y*4;
//					printf("%lf\n", yStart);
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
		
		void HLine()
		{
			pos.y += ymargin;
			DrawLine(GetDrawContext(), pos.x, pos.y, pos.x+rect.width-(showScrollBar ? scrollBarWidth : 0), pos.y);
//			totalHeight += ymargin;
		}
	};

	
	static IMGUIContext* g_IMContext = nullptr;
	
	IMWidget::IMWidget(const char* name)
		: Widget(name)
	{
		m_imContext = new IMGUIContext;
		m_imContext->widget = this;
		m_imContext->Reset();
	}
	
	IMWidget::~IMWidget()
	{
		delete m_imContext;
	}
	
	void IMWidget::Draw()
	{
		auto& r = m_rect;
		auto vg = GetNVGContext();
		nvgIntersectScissor(vg, r.x, r.y, r.width, r.height);
		g_IMContext = m_imContext;
		m_imContext->BeginFrame();
		this->DrawImpl();
		m_imContext->EndFrame();
		g_IMContext = nullptr;
		nvgResetScissor(vg);
	}
	
//#define g_IMContext Context::GetInstance().imContext
	
	Rect NewLine(int height)
	{
		return g_IMContext->NextCell(height);
	}
	
	void Indent(int indent_w)
	{
		g_IMContext->AddIndent(indent_w);
	}
	
	void Unindent(int indent_w)
	{
		g_IMContext->AddIndent(-indent_w);
	}
	
	void Group(const std::string & name)
	{
		Label(name);
		g_IMContext->AddIndent(IMGUIContext::Indent);
	}
	
	void EndGroup()
	{
		g_IMContext->AddIndent(-IMGUIContext::Indent);
		g_IMContext->HLine();
	}
	
	bool Button(const std::string & text)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		auto r = g_IMContext->NextCell(20);
		DrawButton(ctx, 0, text.c_str(), r);
		return false;
	}
	
	bool Button(const std::string & text, const Rect& rect)
	{
		auto ctx = GetDrawContext();
		DrawButton(ctx, 0, text.c_str(), rect);
		return false;
	}
	
	
	void Label(const std::string & text)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		auto r = g_IMContext->NextCell();
		DrawLabel(ctx, text.c_str(), r);
	}
	
	void Label(const std::string & text, const Rect& rect)
	{
		auto ctx = GetDrawContext();
		DrawLabel(ctx, text.c_str(), rect);
	}

	void CheckBox(const std::string & label, bool& inoutValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2);
		DrawLabel(ctx, label.c_str(), r1);
		DrawCheckBox(ctx, r2);
	}
	
	void InputText(const std::string & label, std::string& inoutValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2);
		DrawLabel(ctx, label.c_str(), r1);
		DrawEditBox(ctx, inoutValue.c_str(), r2);
	}
	
	bool Slider(const std::string & label, float& inoutValue, float minValue, float maxValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2);
		DrawLabel(ctx, label.c_str(), r1);
		
		float pos = (inoutValue - minValue) / (maxValue - minValue);
		constexpr int pad = 2;
		constexpr int boxWidth = 50;
		auto v_str = ToString(inoutValue);
		Rect r21, r22;
		HSplitRect2(r2, r21, r22, -1, boxWidth, pad);
		DrawSlider(ctx, pos, r21);
		DrawEditBox(ctx, v_str.c_str(), r22);
		return false;
	}
	
	void Combox(const std::string & label, const std::string& inoutValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2);
		DrawLabel(ctx, label.c_str(), r1);
		
		DrawDropDown(ctx, inoutValue.c_str(), r2);
	}
	
	void Float3(const std::string & label, float& x, float& y, float& z)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		g_IMContext->NextCell2(r1, r2, 1, 3);
		DrawLabel(ctx, label.c_str(), r1);
		
		constexpr int pad = 2;
		constexpr int label_len = 11;
		
		auto x_str = ToString(x);
		auto y_str = ToString(y);
		auto z_str = ToString(z);
		
		Rect r21, r22, r23;
		HSplitRect3(r2, r21, r22, r23, 1, 1, 1, pad);
		constexpr int align = NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE;
		Rect l, r;
		
		HSplitRect2(r21, l, r, label_len, -1, pad);
		DrawLabel(ctx, "X", l, align);
		DrawEditBox(ctx, x_str.c_str(), r);
		
		HSplitRect2(r22, l, r, label_len, -1, pad);
		DrawLabel(ctx, "Y", l, align);
		DrawEditBox(ctx, y_str.c_str(), r);
		
		HSplitRect2(r23, l, r, label_len, -1, pad);
		DrawLabel(ctx, "Z", l, align);
		DrawEditBox(ctx, z_str.c_str(), r);
	}
	
	void SegmentedButtons(int count, const char* labels[], int icons[], const Rect& rect)
	{
		auto ctx = GetDrawContext();
		DrawSegmentedButtons(ctx, count, labels, icons, rect);
	}
}
