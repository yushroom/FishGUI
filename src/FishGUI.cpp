#ifdef _WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#define GLFW_INCLUDE_GLCOREARB
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <FishGUI/Draw.hpp>
#define NANOVG_GL3 1
#include "nanovg_gl.h"
#include <FishGUI/Theme.hpp>

#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Input.hpp>
#include <FishGUI/Window.hpp>
#include <FishGUI/Widget.hpp>

#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

//#include <boost/filesystem/path.hpp>
//#include <boost/filesystem/operations.hpp>
#include <FishGUI/Utils.hpp>

using namespace std::chrono_literals;
//namespace fs = boost::filesystem;

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

	void StatusBar::Draw()
	{
//		auto ctx = m_context->m_nvgContext;

		auto input = Input::GetCurrent();
		std::ostringstream sout;
		sout << "pos(" << input->m_mousePosition.x << ", " << input->m_mousePosition.y << ")";
//		nvgFontSize(ctx, 16.0f);
//		nvgFontFace(ctx, "sans");
//		nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
//		nvgFillColor(ctx, theme.textColor);
//		nvgText(ctx, m_rect.x + 5, m_rect.y + m_rect.height, sout.str().c_str(), nullptr);
		auto r = m_rect;
		r.x += 5;
		Label(sout.str(), r);
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
		auto content = m_children[m_activeTabId];
		content->SetRect(r.x, r.y, r.width, r.height);
		content->BindAndDraw();
		
		nvgRestore(vg);
	}
	
	
	void Splitter::Draw3() const
	{
		auto ctx = GetDrawContext();
		DrawRect(ctx, m_rect, Theme::GetDefaultTheme()->windowBackgroundColor);
//		DrawRect(ctx, m_rect, nvgRGB(0, 255, 0));
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

//		auto full_path = fs::current_path();
		auto resourcesRoot = ApplicationFilePath();
		auto fontsRoot = resourcesRoot + "/fonts/";
		nvgCreateFont(context.m_nvgContext, "icons", 	(fontsRoot+"entypo.ttf").c_str());
		nvgCreateFont(context.m_nvgContext, "sans", 	(fontsRoot+"Roboto-Regular.ttf").c_str());
		nvgCreateFont(context.m_nvgContext, "sans-bold", (fontsRoot+"Roboto-Bold.ttf").c_str());
		nvgCreateFont(context.m_nvgContext, "emoji", 	(fontsRoot+"NotoEmoji-Regular.ttf").c_str());
		nvgCreateFont(context.m_nvgContext, "ui", (fontsRoot+"icomoon.ttf").c_str());

		Cursor::GetInstance().Init();
		
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
			//printf("time: %lf ms\n", interval*1000);
//			constexpr int target_fps = 30;
//			constexpr double target_interval = 1.0 / target_fps;
//			if (interval < target_interval)
//			{
////				std::cout << "time: " << interval << std::endl;
//				std::this_thread::sleep_for((target_interval-interval)*1000ms);
//			}
			lastTime = now;
			//glCheckError();
		}
		
		glfwDestroyWindow(mainWindow->GetGLFWWindow());
		glfwTerminate();
	}

	
	float Clamp(float v, float minv, float maxv)
	{
		assert(minv <= maxv);
		if (v < minv)
			return minv;
		if (v > maxv)
			return maxv;
		return v;
	}
	
	// horizontally divide rect into 2 rects, left & right
	//  if size1 > 0 and size2 > 0:	width of left = (r.width - interval) * size1 / (size1+size2))
	//  if size1 == -1, then right has fixed width size2
	void HSplitRect2(const Rect& r, Rect& r1, Rect& r2, int size1, int size2, int interval)
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
	
		
	void IMGUIContext::EndFrame()
	{
		totalHeight = static_cast<int>( pos.y - (yStart+rect.y) );
		if (totalHeight > rect.height)
		{
			needScrollBar = true;
		}
//			Label("yStart="+ToString(yStart)+" pos.y="+std::to_string(pos.y) + " rect.bottom="+std::to_string(rect.y+rect.height));
//			Label("yStart="+ToString(yStart)+" pos.y="+std::to_string(pos.y) + " totalHeight="+std::to_string(totalHeight));
		auto ctx = GetDrawContext();
		auto theme = CurrentTheme();
		if (showScrollBar)
		{
			int x = rect.x+rect.width-scrollBarWidth;
			int y = rect.y;
			int w = scrollBarWidth;
			int h = rect.height;
			DrawRect(ctx, (float)x, (float)y, (float)w, (float)h, theme->scrollBarBackColor);
			
//				const int total_height = pos.y - (yStart+rect.y);
			float p = float(rect.height) / totalHeight;
			constexpr int pad = 1;
			x = rect.x+rect.width-scrollBarWidth+pad;
//				y = rect.y + float(rect.height)*(-yStart)/total_height;
			y = static_cast<int>( rect.y + (-yStart)*p );
			w = scrollBarWidth - pad*2;
			h = static_cast<int>(p * rect.height);
			int r = scrollBarWidth / 2 - pad;
			DrawRoundedRect(ctx, (float)x, (float)y, (float)w, (float)h, (float)r, theme->scrollBarColor);
			
			auto input = Input::GetCurrent();
			if (input->m_scrolling && input->MouseInRect(rect))
			{
				yStart += input->m_scroll.y*4;
//					printf("%lf\n", yStart);
				yStart = Clamp(yStart, float(rect.height - totalHeight), 0);
			}
		}
	}
		
	Rect IMGUIContext::NextCell(int height, bool& outOfRange)
	{
		auto ret = rect;
		ret.x = pos.x + xmargin + indent;
		ret.y = pos.y + ymargin;
		ret.width -= xmargin*2 + indent + (showScrollBar ? scrollBarWidth : 0);
		ret.height = height;
		pos.y += ymargin + height;
		outOfRange =
			ret.top() >= rect.bottom() ||
			ret.bottom() <= rect.top() ||
			ret.left() >= rect.right() ||
			ret.right() <= rect.left();
		
		return ret;
	}
		
		// label + ...
	void IMGUIContext::NextCell2(Rect& left, Rect& right, bool& outOfRange, float leftLen, float rightLen, int height)
	{
		auto r = NextCell(cellHeight, outOfRange);
		HSplitRect2(r, left, right, leftLen, rightLen, xmargin);
	}
		
	void IMGUIContext::AddIndent(int indent)
	{
		this->indent += indent;
	}
		
	void IMGUIContext::HLine()
	{
		pos.y += ymargin;
		DrawLine(GetDrawContext(), pos.x, pos.y, pos.x+rect.width-(showScrollBar ? scrollBarWidth : 0), pos.y, nvgRGB(0, 0, 0));
//			totalHeight += ymargin;
	}

	
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
		nvgIntersectScissor(vg, (float)r.x, (float)r.y, (float)r.width, (float)r.height);
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
		bool outOfRange;
		return g_IMContext->NextCell(height, outOfRange);
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
		assert(g_IMContext != nullptr);
		bool outOfRange;
		auto r = g_IMContext->NextCell(20, outOfRange);
		if (outOfRange)
			return false;
		return Button(text, r);
	}
	
	bool Button(const std::string & text, const Rect& rect)
	{
		auto ctx = GetDrawContext();
		DrawButton(ctx, text.c_str(), rect);
		NVGcolor color1 = CurrentTheme()->buttonGradientTopUnfocused;
		NVGcolor color2 = CurrentTheme()->buttonGradientBotUnfocused;
		bool ret = false;
		if (CurrentInput()->MouseInRect(rect))
		{
			auto state = CurrentInput()->GetMouseButtonState(MouseButton::Left);
			bool pressed = (state != MouseButtonState::None);
			if (pressed)
			{
				color1 = CurrentTheme()->buttonGradientTopPushed;
				color2 = CurrentTheme()->buttonGradientBotPushed;
//				color1 = CurrentTheme()->buttonGradientTopFocused;
//				color2 = CurrentTheme()->buttonGradientBotFocused;
			}
			
			ret = (state == MouseButtonState::Up);
		}
		DrawButton(ctx, text.c_str(), rect, color1, color2);
		return ret;
	}
	
	
	void Label(const std::string & text)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		bool outOfRange;
		auto r = g_IMContext->NextCell(IMGUIContext::cellHeight, outOfRange);
		if (outOfRange)
			return;
		DrawLabel(ctx, text.c_str(), r);
	}
	
	void Label(const std::string & text, const Rect& rect)
	{
		auto ctx = GetDrawContext();
		DrawLabel(ctx, text.c_str(), rect);
	}

	bool CheckBox(const std::string & label, bool& inoutValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		bool outOfRange;
		g_IMContext->NextCell2(r1, r2, outOfRange);
		if (outOfRange)
			return false;
		DrawLabel(ctx, label.c_str(), r1);
		bool ret = false;
		if (CurrentInput()->MouseInRect(r2))
		{
			if (CurrentInput()->GetMouseButton(MouseButton::Left))
			{
//				color1 = CurrentTheme()->buttonGradientTopPushed;
//				color2 = CurrentTheme()->buttonGradientBotPushed;
			}
			else if (CurrentInput()->GetMouseButtonUp(MouseButton::Left))
			{
				ret = true;
				inoutValue = !inoutValue;
			}
		}
		DrawCheckBox(ctx, r2, inoutValue);
		return ret;
	}
	
	void InputText(const std::string & label, std::string& inoutValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		bool outOfRange;
		g_IMContext->NextCell2(r1, r2, outOfRange);
		if (outOfRange)
			return;
		DrawLabel(ctx, label.c_str(), r1);
		DrawEditBox(ctx, inoutValue.c_str(), r2);
	}
	
	bool Slider(const std::string & label, float& inoutValue, float minValue, float maxValue)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		bool outOfRange;
		g_IMContext->NextCell2(r1, r2, outOfRange);
		if (outOfRange)
			return false;
		
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
		bool outOfRange;
		g_IMContext->NextCell2(r1, r2, outOfRange);
		if (outOfRange)
			return;
		
		DrawLabel(ctx, label.c_str(), r1);
		
		DrawDropDown(ctx, inoutValue.c_str(), r2);
	}
	
	void Image(unsigned int image, int width, int height, bool flip)
	{
		assert(g_IMContext->widget != nullptr);
//		auto ctx = GetDrawContext();
		bool outOfRange;
		auto r = g_IMContext->NextCell(height, outOfRange);
		r.width = width;
		Image(image, r, flip);
	}
	
	void Image(unsigned int image, const Rect& r, bool flip)
	{
		auto ctx = GetDrawContext();
		DrawImage(ctx, image, r, flip);
	}
	
	void Float3(const std::string & label, float& x, float& y, float& z)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		bool outOfRange;
		g_IMContext->NextCell2(r1, r2, outOfRange, 1, 3);
		if (outOfRange)
			return;
		
		DrawLabel(ctx, label.c_str(), r1);
		
		constexpr int pad = 2;
		constexpr int label_len = 8;
		
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
	
	/*
	void DrawSegmentedButtons(DrawContext* context, int count, SegmentedButton buttons[], float x, float y, float w, float h)
	{
		constexpr float cornerRadius = 4.0f;
		int width = w / count;
		auto theme = CurrentTheme();
		auto ctx = GetNVGContext();
		NVGcolor colorTop = theme->buttonGradientTopUnfocused;
		NVGcolor colorBot = theme->buttonGradientBotUnfocused;
		NVGpaint bg = nvgLinearGradient(ctx, x, y, x, y + h, colorTop, colorBot);
		nvgBeginPath(ctx);
		nvgRoundedRect(ctx, x + 1, y + 1, w - 2, h - 2, cornerRadius - 1);
		nvgFillPaint(ctx, bg);
		nvgFill(ctx);
		
		auto input = CurrentInput();
		Rect r;
		r.x = x;
		r.y = y;
		r.width = width;
		r.height = h;
		
		// background
		for (int i = 0; i < count; ++i)
		{
			bool inside = input->MouseInRect(r);
			bool clicked = inside && input->GetMouseButtonUp(MouseButton::Left);
			bool pushed = clicked || (inside && (input->GetMouseButton(MouseButton::Left)));
			buttons[i].outClicked = clicked;
			if (clicked)
				buttons[i].active = !buttons[i].active;
			r.x += width;
			
			if (!buttons[i].active)
				continue;
			nvgScissor(ctx, x+width*i, y, width, h);
			colorTop = theme->buttonGradientTopFocused;
			colorBot = theme->buttonGradientBotFocused;
			if (pushed)
			{
				colorTop = theme->buttonGradientTopPushed;
				colorBot = theme->buttonGradientBotPushed;
			}
			bg = nvgLinearGradient(ctx, x, y, x, y + h, colorTop, colorBot);
			// nvgBeginPath(ctx);
			nvgFillPaint(ctx, bg);
			nvgFill(ctx);
			nvgResetScissor(ctx);
		}
		
		auto black = nvgRGB(0, 0, 0);
		nvgBeginPath(ctx);
		nvgRoundedRect(ctx, x + 0.5f, y + 0.5f, w - 1, h - 1, cornerRadius - 0.5f);
		nvgStrokeColor(ctx, black);
		nvgStroke(ctx);
		
		for (int i = 1; i < count; ++i)
		{
			int xx = x + width*i;
			DrawLine(context, xx, y, xx, y+h, black);
		}
		
//		nvgFontSize(ctx, theme->standardFontSize);
//		nvgFontFace(ctx, "ui");
//		nvgFillColor(ctx, theme->textColor);
		nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		y += h*0.5f;
		x += width/2;
		for (int i = 0; i < count; ++i)
		{
			nvgFillColor(ctx, buttons[i].fontColor);
			nvgText(ctx, x, y, buttons[i].text, NULL);
			x += width;
			r.x += width;
		}
	}
	
	void SegmentedButtons(int count, SegmentedButton buttons[], const Rect& r)
	{
		auto ctx = GetDrawContext();
		DrawSegmentedButtons(ctx, count, buttons, r.x, r.y, r.width, r.height);
	}
	 */
}
