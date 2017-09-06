#include "RFGUI.hpp"

#include "nanovg.h"
//#define NANOVG_GL3
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include <vector>
#include <map>
#include <stack>
#include <cassert>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#define ICON_SEARCH 0x1F50D
#define ICON_CIRCLED_CROSS 0x2716
#define ICON_CHEVRON_RIGHT 0xE75E
#define ICON_CHECK 0x2713
#define ICON_LOGIN 0xE740
#define ICON_TRASH 0xE729


void _checkOpenGLError(const char *file, int line)
{
	GLenum err(glGetError());
	
	while (err != GL_NO_ERROR)
	{
		std::string error;
		
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
		printf("GL_%s - %s:%d\n", error.c_str(), file, line);
		err = glGetError();
	}
}

#define glCheckError() _checkOpenGLError(__FILE__, __LINE__)

template<typename T>
inline std::string StringCast (T const & t) {
	std::ostringstream sout;
	sout << t;
	return sout.str();
}

// Returns 1 if col.rgba is 0.0f,0.0f,0.0f,0.0f, 0 otherwise
int isBlack(NVGcolor col)
{
	if( col.r == 0.0f && col.g == 0.0f && col.b == 0.0f && col.a == 0.0f )
	{
		return 1;
	}
	return 0;
}

static char* cpToUTF8(int cp, char* str)
{
	int n = 0;
	if (cp < 0x80) n = 1;
	else if (cp < 0x800) n = 2;
	else if (cp < 0x10000) n = 3;
	else if (cp < 0x200000) n = 4;
	else if (cp < 0x4000000) n = 5;
	else if (cp <= 0x7fffffff) n = 6;
	str[n] = '\0';
	switch (n) {
		case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
		case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
		case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
		case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
		case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
		case 1: str[0] = cp;
	}
	return str;
}

void drawWindow(NVGcontext* vg, const char* title, float x, float y, float w, float h)
{
	float cornerRadius = 3.0f;
	NVGpaint shadowPaint;
	NVGpaint headerPaint;
	
	nvgSave(vg);
	//	nvgClearState(vg);
	
	// Window
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x,y, w,h, cornerRadius);
	nvgFillColor(vg, nvgRGBA(28,30,34,192));
	//	nvgFillColor(vg, nvgRGBA(0,0,0,128));
	nvgFill(vg);
	
	// Drop shadow
	shadowPaint = nvgBoxGradient(vg, x,y+2, w,h, cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
	nvgBeginPath(vg);
	nvgRect(vg, x-10,y-10, w+20,h+30);
	nvgRoundedRect(vg, x,y, w,h, cornerRadius);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, shadowPaint);
	nvgFill(vg);
	
	if (title) {
		// Header
		headerPaint = nvgLinearGradient(vg, x,y,x,y+15, nvgRGBA(255,255,255,8), nvgRGBA(0,0,0,16));
		nvgBeginPath(vg);
		nvgRoundedRect(vg, x+1,y+1, w-2,30, cornerRadius-1);
		nvgFillPaint(vg, headerPaint);
		nvgFill(vg);
		nvgBeginPath(vg);
		nvgMoveTo(vg, x+0.5f, y+0.5f+30);
		nvgLineTo(vg, x+0.5f+w-1, y+0.5f+30);
		nvgStrokeColor(vg, nvgRGBA(0,0,0,32));
		nvgStroke(vg);
		
		nvgFontSize(vg, 18.0f);
		nvgFontFace(vg, "sans-bold");
		nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
		
		nvgFontBlur(vg,2);
		nvgFillColor(vg, nvgRGBA(0,0,0,128));
		nvgText(vg, x+w/2,y+16+1, title, NULL);
		
		nvgFontBlur(vg,0);
		nvgFillColor(vg, nvgRGBA(220,220,220,160));
		nvgText(vg, x+w/2,y+16, title, NULL);
	}
	
	nvgRestore(vg);
}


void drawSearchBox(NVGcontext* vg, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = h/2-1;
	
	// Edit
	bg = nvgBoxGradient(vg, x,y+1.5f, w,h, h/2,5, nvgRGBA(0,0,0,16), nvgRGBA(0,0,0,92));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x,y, w,h, cornerRadius);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	
	/*	nvgBeginPath(vg);
	 nvgRoundedRect(vg, x+0.5f,y+0.5f, w-1,h-1, cornerRadius-0.5f);
	 nvgStrokeColor(vg, nvgRGBA(0,0,0,48));
	 nvgStroke(vg);*/
	
	nvgFontSize(vg, h*1.3f);
	nvgFontFace(vg, "icons");
	nvgFillColor(vg, nvgRGBA(255,255,255,64));
	nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+h*0.55f, y+h*0.55f, cpToUTF8(ICON_SEARCH,icon), NULL);
	
	nvgFontSize(vg, 20.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255,255,255,32));
	
	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+h*1.05f,y+h*0.5f,text, NULL);
	
	nvgFontSize(vg, h*1.3f);
	nvgFontFace(vg, "icons");
	nvgFillColor(vg, nvgRGBA(255,255,255,32));
	nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+w-h*0.55f, y+h*0.55f, cpToUTF8(ICON_CIRCLED_CROSS,icon), NULL);
}

void drawDropDown(NVGcontext* vg, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = 4.0f;
	
	bg = nvgLinearGradient(vg, x,y,x,y+h, nvgRGBA(255,255,255,16), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+1,y+1, w-2,h-2, cornerRadius-1);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+0.5f,y+0.5f, w-1,h-1, cornerRadius-0.5f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,48));
	nvgStroke(vg);
	
	nvgFontSize(vg, 20.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255,255,255,160));
	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+h*0.3f,y+h*0.5f,text, NULL);
	
	nvgFontSize(vg, h*1.3f);
	nvgFontFace(vg, "icons");
	nvgFillColor(vg, nvgRGBA(255,255,255,64));
	nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+w-h*0.5f, y+h*0.5f, cpToUTF8(ICON_CHEVRON_RIGHT,icon), NULL);
}

void drawLabel(NVGcontext* vg, const char* text, float x, float y, float w, float h)
{
	NVG_NOTUSED(w);
	
	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255,255,255,128));
	
	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	nvgText(vg, x,y+h*0.5f,text, NULL);
}

void drawEditBoxBase(NVGcontext* vg, float x, float y, float w, float h)
{
	NVGpaint bg;
	// Edit
	bg = nvgBoxGradient(vg, x+1,y+1+1.5f, w-2,h-2, 3,4, nvgRGBA(255,255,255,32), nvgRGBA(32,32,32,32));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+1,y+1, w-2,h-2, 4-1);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+0.5f,y+0.5f, w-1,h-1, 4-0.5f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,48));
	nvgStroke(vg);
}

void drawEditBox(NVGcontext* vg, const char* text, float x, float y, float w, float h)
{
	
	drawEditBoxBase(vg, x,y, w,h);
	
	nvgFontSize(vg, 16.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255,255,255,64));
	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+h*0.3f,y+h*0.5f,text, NULL);
}


void drawCheckBox(NVGcontext* vg, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	NVG_NOTUSED(w);
	
	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255,255,255,160));
	
	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+28,y+h*0.5f,text, NULL);
	
	bg = nvgBoxGradient(vg, x+1,y+(int)(h*0.5f)-9+1, 18,18, 3,3, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,92));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+1,y+(int)(h*0.5f)-9, 18,18, 3);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	
	nvgFontSize(vg, 40);
	nvgFontFace(vg, "icons");
	nvgFillColor(vg, nvgRGBA(255,255,255,128));
	nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
	nvgText(vg, x+9+2, y+h*0.5f, cpToUTF8(ICON_CHECK,icon), NULL);
}


void drawButton(NVGcontext* vg, int preicon, const char* text, float x, float y, float w, float h, NVGcolor colTop, NVGcolor colBot)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = 4.0f;
	float tw = 0, iw = 0;
	
//	bg = nvgLinearGradient(vg, x,y,x,y+h, nvgRGBA(255,255,255,isBlack(col)?16:32), nvgRGBA(0,0,0,isBlack(col)?16:32));
	bg = nvgLinearGradient(vg, x,y,x,y+h, colTop, colBot);
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+1,y+1, w-2,h-2, cornerRadius-1);
//	if (!isBlack(col)) {
//		nvgFillColor(vg, col);
//		nvgFill(vg);
//	}
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+0.5f,y+0.5f, w-1,h-1, cornerRadius-0.5f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,48));
	nvgStroke(vg);
	
	nvgFontSize(vg, 20.0f);
	nvgFontFace(vg, "sans-bold");
	tw = nvgTextBounds(vg, 0,0, text, NULL, NULL);
	if (preicon != 0) {
		nvgFontSize(vg, h*1.3f);
		nvgFontFace(vg, "icons");
		iw = nvgTextBounds(vg, 0,0, cpToUTF8(preicon,icon), NULL, NULL);
		iw += h*0.15f;
	}
	
	if (preicon != 0) {
		nvgFontSize(vg, h*1.3f);
		nvgFontFace(vg, "icons");
		nvgFillColor(vg, nvgRGBA(255,255,255,96));
		nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
		nvgText(vg, x+w*0.5f-tw*0.5f-iw*0.75f, y+h*0.5f, cpToUTF8(preicon,icon), NULL);
	}
	
	nvgFontSize(vg, 20.0f);
	nvgFontFace(vg, "sans-bold");
	nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
	nvgFillColor(vg, nvgRGBA(0,0,0,160));
	nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f-1,text, NULL);
	nvgFillColor(vg, nvgRGBA(255,255,255,160));
	nvgText(vg, x+w*0.5f-tw*0.5f+iw*0.25f,y+h*0.5f,text, NULL);
}

void drawSlider(NVGcontext* vg, float pos, float x, float y, float w, float h)
{
	NVGpaint bg, knob;
	float cy = y+(int)(h*0.5f);
	float kr = (int)(h*0.25f);
	
	nvgSave(vg);
	//	nvgClearState(vg);
	
	// Slot
	bg = nvgBoxGradient(vg, x,cy-2+1, w,4, 2,2, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,128));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x,cy-2, w,4, 2);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	
	// Knob Shadow
	bg = nvgRadialGradient(vg, x+(int)(pos*w),cy+1, kr-3,kr+3, nvgRGBA(0,0,0,64), nvgRGBA(0,0,0,0));
	nvgBeginPath(vg);
	nvgRect(vg, x+(int)(pos*w)-kr-5,cy-kr-5,kr*2+5+5,kr*2+5+5+3);
	nvgCircle(vg, x+(int)(pos*w),cy, kr);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, bg);
	nvgFill(vg);
	
	// Knob
	knob = nvgLinearGradient(vg, x,cy-kr,x,cy+kr, nvgRGBA(255,255,255,16), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgCircle(vg, x+(int)(pos*w),cy, kr-1);
	nvgFillColor(vg, nvgRGBA(40,43,48,255));
	nvgFill(vg);
	nvgFillPaint(vg, knob);
	nvgFill(vg);
	
	nvgBeginPath(vg);
	nvgCircle(vg, x+(int)(pos*w),cy, kr-0.5f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,92));
	nvgStroke(vg);
	
	nvgRestore(vg);
}


namespace RFGUI {
	
	enum class KeyState {
		Up = 0,
		Down = 1,
		Held = 2,
		None = 3,
	};
	
	enum class MouseButtonState {
		None = 0,
		Down = 1,
		Held = 2,
		Up = 3,
	};
	
	enum class MouseButtonCode {
		Left = 0,
		Right = 1,
		Middle = 2,
	};
	
	
	class Input {
	public:
		
		static void Init()
		{
			for (auto& s : m_mouseButtonStates) {
				s = MouseButtonState::None;
			}
		}
		
		static void Update()
		{
			for (auto& s : m_mouseButtonStates) {
				if (s == MouseButtonState::Down)
					s = MouseButtonState::Held;
				else if (s == MouseButtonState::Up)
					s = MouseButtonState::None;
			}
		}
		
		// Returns whether the given mouse button is held down.
		// button values are 0 for left button, 1 for right button, 2 for the middle button.
		static bool GetMouseButton(MouseButtonCode button)
		{
			return m_mouseButtonStates[static_cast<int>(button)] == MouseButtonState::Held;
		}
		
		// Returns true during the frame the user pressed the given mouse button.
		// button values are 0 for left button, 1 for right button, 2 for the middle button.
		static bool GetMouseButtonDown(MouseButtonCode button)
		{
			return m_mouseButtonStates[static_cast<int>(button)] == MouseButtonState::Down;
		}
		
		// Returns true during the frame the user releases the given mouse button.
		// button values are 0 for left button, 1 for right button, 2 for the middle button.
		static bool GetMouseButtonUp(MouseButtonCode button)
		{
			return m_mouseButtonStates[static_cast<int>(button)] == MouseButtonState::Up;
		}
		
		//	private:
		static MouseButtonState m_mouseButtonStates[3];
		static Vector2 m_mousePosition;	// normalized in range [0, 1]
		static bool m_inputMode;
		static std::string m_stringBuffer;
//		static std::map<GLFWwindow*, Input> s_windowToInput;
	};
	
	MouseButtonState	Input::m_mouseButtonStates[3];
	Vector2				Input::m_mousePosition = {0, 0};
	bool				Input::m_inputMode = false;
	std::string			Input::m_stringBuffer;
	
	constexpr NVGcolor Color(int intensity, int alpha)
	{
//		NVGcolor c;
//		c.r = c.g = c.b = intensity/255.0f;
//		c.a = alpha / 255.0f;
//		return c;
		return NVGcolor{intensity/255.0f, intensity/255.0f, intensity/255.0f, alpha/255.0f};
	}
	
	struct Theme {
		int mStandardFontSize                 = 16;
		int mButtonFontSize                   = 20;
		int mTextBoxFontSize                  = 20;
		int mWindowCornerRadius               = 2;
		int mWindowHeaderHeight               = 30;
		int mWindowDropShadowSize             = 10;
		int mButtonCornerRadius               = 2;
		float mTabBorderWidth                 = 0.75f;
		int mTabInnerMargin                   = 5;
		int mTabMinButtonWidth                = 20;
		int mTabMaxButtonWidth                = 160;
		int mTabControlWidth                  = 20;
		int mTabButtonHorizontalPadding       = 10;
		int mTabButtonVerticalPadding         = 2;
		
		NVGcolor mDropShadow					   = Color(0, 128);
		NVGcolor mTransparent                      = Color(0, 0);
		NVGcolor mBorderDark                       = Color(29, 255);
		NVGcolor mBorderLight                      = Color(92, 255);
		NVGcolor mBorderMedium                     = Color(35, 255);
		NVGcolor mTextColor                        = Color(255, 160);
		NVGcolor mDisabledTextColor                = Color(255, 80);
		NVGcolor mTextColorShadow                  = Color(0, 160);
		NVGcolor mIconColor                        = mTextColor;
		
		NVGcolor mButtonGradientTopFocused         = Color(64, 255);
		NVGcolor mButtonGradientBotFocused         = Color(48, 255);
		NVGcolor mButtonGradientTopUnfocused       = Color(74, 255);
		NVGcolor mButtonGradientBotUnfocused       = Color(58, 255);
		NVGcolor mButtonGradientTopPushed          = Color(41, 255);
		NVGcolor mButtonGradientBotPushed          = Color(29, 255);
		
		/* Window-related */
		NVGcolor mWindowFillUnfocused              = Color(43, 230);
		NVGcolor mWindowFillFocused                = Color(45, 230);
		NVGcolor mWindowTitleUnfocused             = Color(220, 160);
		NVGcolor mWindowTitleFocused               = Color(255, 190);
		
		NVGcolor mWindowHeaderGradientTop          = mButtonGradientTopUnfocused;
		NVGcolor mWindowHeaderGradientBot          = mButtonGradientBotUnfocused;
		NVGcolor mWindowHeaderSepTop               = mBorderLight;
		NVGcolor mWindowHeaderSepBot               = mBorderDark;
		
		NVGcolor mWindowPopup                      = Color(50, 255);
		NVGcolor mWindowPopupTransparent           = Color(50, 0);
	};
	
	static Theme g_theme;
	
	struct WindowManager
	{
		GLFWwindow* m_mainWindow = nullptr;
		std::vector<GLFWwindow*> m_windows;
		std::vector<GLFWwindow*> m_pool;
		std::stack<GLFWwindow*> m_windowStack;
		
		static WindowManager& Instance()
		{
			static WindowManager mgr;
			return mgr;
		}
		
		GLFWwindow * mainWindow() const
		{
			return m_mainWindow;
		}
		
//		Window* ActiveWindow();
//		Window* UnusedWindow() {
//			
//		}
		
		GLFWwindow * focusedWindow()
		{
			for (auto w : m_windows)
			{
//				if (glfwgetfo)
			}
		}
		
		void PushWindow(GLFWwindow* window)
		{
			m_windowStack.push(window);
		}
		
		GLFWwindow* PopWindow()
		{
			if (m_windowStack.empty())
				return nullptr;
			auto m = m_windowStack.top();
			m_windowStack.pop();
			return m;
		}
	};
	
	struct CenterScreenState
	{
		int x = 0;
		int y = 0;
		int w = 1;
		int h = 1;
		
		void reset() {
			x = 0;
			y = 0;
			w = g_renderContext.width;
			h = g_renderContext.height;
		}
		
		void addTab(TabPosition tabPosition, int widthOrHeight, Rect * rect)
		{
			if (tabPosition == TabPosition::Left) {
//				assert(widthOrHeight <= w);
				rect->x = x;
				rect->y = y;
				rect->width = widthOrHeight;
				rect->height = h;
				x += widthOrHeight;
				w -= widthOrHeight;
			} else if (tabPosition == TabPosition::Right) {
//				assert(widthOrHeight <= w);
				rect->x = x+w-widthOrHeight;
				rect->y = y;
				rect->width = widthOrHeight;
				rect->height = h;
				w -= widthOrHeight;
			} else if (tabPosition == TabPosition::Top) {
//				assert(widthOrHeight <= h);
				rect->x = x;
				rect->y = y;
				rect->width = w;
				rect->height = widthOrHeight;
				y += widthOrHeight;
				h -= widthOrHeight;
			} else if (tabPosition == TabPosition::Bottom) {
//				assert(widthOrHeight <= h);
				rect->x = x;
				rect->y = y+h-widthOrHeight;
				rect->width = w;
				rect->height = widthOrHeight;
				h -= widthOrHeight;
			} else { // floating
				rect->x = x;
				rect->y = y;
				rect->width = w;
				rect->height = h;
				w = 0;
				h = 0;
			}
		}
	};
	
	static CenterScreenState g_centerScreen;
	
	struct TabState
	{
		Rect rect;
		const int x_margin_left = 10;
		int x_margin_right = 10; // or 18
		const int y_margin = 5;
		const int y_cell_height = 20;
//		int y_start = 0;
		int y_filled = 0;
		int cell_count = 0;
//		int hover_cell_id = -1;
//		int clicked_cell_id = -1;
		
		void reset() {
			rect.x = 0;
			rect.y = 0;
			y_filled = 0;
			cell_count = 0;
		}
		
		void nextCellOrigin(int *px, int *py) {
			*px = rect.x + x_margin_left;
			*py = rect.y + y_filled + y_margin;
		}
		
		int get_avaliable_width() {
			return rect.width - x_margin_left - x_margin_right;
		}
		
		int get_current_cell_id() {
			return cell_count;
		}
		
		// cell height without margin
		void add_cell(const int height) {
			y_filled += height + y_margin;
			cell_count ++;
		}
	};
//	static TabState g_sideBarState;
	
	CursorType g_cursorType;
	GLFWcursor* g_cursors[(int)CursorType::CursorCount];
	void SetCursorType(CursorType type)
	{
		if (g_cursorType == type) {
			return;
		}
		g_cursorType = type;
		glfwSetCursor(g_renderContext.window, g_cursors[static_cast<int>(type)]);
	}
	
	struct Window
	{
		std::list<Tab*>		m_tabs;
		GLFWwindow*			m_window;
		GLNVGvertexBuffers* m_buffers;
		bool				m_focused = false;
		
		~Window()
		{
			for (auto t : m_tabs)
				delete t;
			m_tabs.clear();
			free(m_buffers);
		}
	};
	
	static Window g_mainWindow;
	static std::list<Window*> g_floatingWindows;
	
	static Window* findWindow(GLFWwindow* w)
	{
		if (g_mainWindow.m_window == w)
			return &g_mainWindow;
		for (auto & window : g_floatingWindows)
		{
			if (window->m_window == w)
				return window;
		}
		return nullptr;
	}
	
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
	
//    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
//    {
//        g_renderContext.mouse_position.x = float(xpos);
//        g_renderContext.mouse_position.y = float(ypos);
//        //printf("mouse position: (%lf, %lf)\n", xpos, ypos);
//    }
	
	void glfw_char_callback(GLFWwindow *window, unsigned int codepoint) {
		std::cout << codepoint << std::endl;
		static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
		std::string u8str = converter.to_bytes(codepoint);
		std::cout << u8str << std::endl;
//		char32_t ch = codepoint;
		Input::m_stringBuffer += u8str;
	}
	
	void glfw_window_resize_callback(GLFWwindow *window, int width, int height)
	{
		if (window == WindowManager::Instance().mainWindow()) {
			g_renderContext.width = width;
			g_renderContext.height = height;
		}
		
		std::cout << "resize" << std::endl;
		g_renderContext.width = width;
		g_renderContext.height = height;
		float ratio;
//		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
	}
	
	void glfw_window_focus_callback(GLFWwindow* window, int focused)
	{
//		std::cout << "glfw_window_focus_callback" << std::endl;
		auto w = findWindow(window);
		w->m_focused = focused;
	}
	
	void glfw_window_iconify_callback(GLFWwindow* window, int iconified)
	{
		if (iconified)
		{
			// The window was iconified
		}
		else
		{
			// The window was restored
		}
	}
	
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
		MouseButtonState state = MouseButtonState::None;
		if (action == GLFW_PRESS)
			state = MouseButtonState::Down;
		else if (action == GLFW_RELEASE)
			state = MouseButtonState::Up;
		
		Input::m_mouseButtonStates[button] = state;
    }
    
    inline bool mouseInRegion(int x, int y, int w, int h) {
		auto mx = Input::m_mousePosition.x;
        auto my = Input::m_mousePosition.y;
        return (mx > x && mx < x+w && my > y && my < y+h);
    }
	
	NVGcontext* vg = NULL;
	

    
    void Init(GLFWwindow * window)
	{
		WindowManager::Instance().m_mainWindow = window;
		g_mainWindow.m_window = window;
//		glfwSetWindowSizeCallback(window, RFGUI::window_size_callback);
//		glfwSetKeyCallback(window, RFGUI::key_callback);
//		glfwSetMouseButtonCallback(window, RFGUI::mouse_button_callback);
//		glfwSetCharCallback(window, RFGUI::glfw_char_callback);
//		glfwSetWindowSizeCallback(window, glfw_window_resize);
//		int width = 0;
//		int height = 0;
//		glfwGetWindowSize(window, &width, &height);
//		glfw_window_resize(window, width, height);
//		
//        g_renderContext.window = window;
		MakeCurrent(window);
		
		vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
		
		nvgCreateFont(vg, "icons", "/Users/yushroom/program/github/nanovg/example/entypo.ttf");
		nvgCreateFont(vg, "sans", "/Users/yushroom/program/github/nanovg/example/Roboto-Regular.ttf");
		nvgCreateFont(vg, "sans-bold", "/Users/yushroom/program/github/nanovg/example/Roboto-Bold.ttf");
		nvgCreateFont(vg, "emoji", "/Users/yushroom/program/github/nanovg/example/NotoEmoji-Regular.ttf");
		
		for (int i = 0; i < static_cast<int>(CursorType::CursorCount); ++i) {
			g_cursors[i] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR + i);
		}
    }
	
	GLFWwindow * CreateGLFWwindow()
	{
		auto window = glfwCreateWindow(256, 512, "dialog", nullptr, g_mainWindow.m_window);
		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCharCallback(window, glfw_char_callback);
		glfwSetWindowSizeCallback(window, glfw_window_resize_callback);
		glfwSetWindowFocusCallback(window, glfw_window_focus_callback);
		WindowManager::Instance().PushWindow(window);
		WindowManager::Instance().m_windows.push_back(window);
		return window;
	}

	Tab* CreateTab(const char* title, TabPosition tabPosition, int widthOrHeight)
	{
		Tab* tab = new Tab(title, tabPosition);
		if (tabPosition == TabPosition::Floating)
		{
			auto w = CreateGLFWwindow();
			auto window = new Window;
			window->m_window = w;
			g_floatingWindows.push_back(window);
			window->m_tabs.push_back(tab);
		}
		else
		{
			g_mainWindow.m_tabs.push_back(tab);
			if (tabPosition == TabPosition::Left || tabPosition == TabPosition::Right)
			{
				tab->m_minimalSize = 128;
			} else {
				tab->m_minimalSize = 56;
			}
		}
//		tab->m_position = tabPosition;
		tab->m_size = widthOrHeight;
		
		return tab;
	}
	

	
	void MakeCurrent(GLFWwindow * window)
	{
		if (g_renderContext.window == window)
			return;
		
		if (g_renderContext.window != nullptr)
			WindowManager::Instance().PushWindow(g_renderContext.window);
		
		glfwMakeContextCurrent(window);
		
//		glfwSetWindowSizeCallback(window, RFGUI::window_size_callback);
		glfwSetKeyCallback(window, RFGUI::key_callback);
		glfwSetMouseButtonCallback(window, RFGUI::mouse_button_callback);
		glfwSetCharCallback(window, RFGUI::glfw_char_callback);
		glfwSetWindowSizeCallback(window, glfw_window_resize_callback);
		glfwSetWindowFocusCallback(window, glfw_window_focus_callback);
//		glfwSetWindowIconifyCallback(window, glfw_window_iconify_callback);
		int width = 0;
		int height = 0;
		glfwGetWindowSize(window, &width, &height);
		glfw_window_resize_callback(window, width, height);
		g_renderContext.window = window;
	}
	
	static TabState g_currentTab;

    
    void Begin()
	{
		SetCursorType(CursorType::Arrow);
		
		
		g_renderContext.isAllWindowInvisiable = true;
		if (g_mainWindow.m_focused)
			g_renderContext.isAllWindowInvisiable = false;
		else
		{
			for (auto& w : g_floatingWindows)
			{
				if (w->m_focused)
				{
					g_renderContext.isAllWindowInvisiable = false;
					break;
				}
			}
		}
		
		if (g_renderContext.isAllWindowInvisiable)
		{
			std::this_thread::sleep_for(33.3ms);
		}
    }
    
    void End()
	{
		Input::Update();
		
		glfwPollEvents();
		
		auto mainWindow = WindowManager::Instance().mainWindow();
		auto windows = WindowManager::Instance().m_windows;
		WindowManager::Instance().m_windows.clear();
		for (auto w : windows)
		{
			if (glfwWindowShouldClose(w))
			{
				glfwDestroyWindow(w);
				g_renderContext.window = mainWindow;
				glfwMakeContextCurrent(mainWindow);
				glfwFocusWindow(mainWindow);
//				auto it = std::find(g_floatingWindows.begin(), g_floatingWindows.end(), [](Window* a, GLFWwindow* b) {
//					return a->m_window == b;
//				});
				
				// fixme: memory leak
				g_floatingWindows.remove_if([w](Window* a) {
					return a->m_window == w;
				});
			}
			else
			{
				WindowManager::Instance().m_windows.push_back(w);
			}
		}
		
//		if (g_renderContext.window != WindowManager::Instance().mainWindow() && glfwWindowShouldClose(g_renderContext.window))
//		{
//			auto w = WindowManager::Instance().PopWindow();
//			if (w == nullptr)
//				w = WindowManager::Instance().mainWindow();
//			if (w != nullptr)
//			{
//				glfwDestroyWindow(g_renderContext.window);
//				g_renderContext.window = w;
//				glfwMakeContextCurrent(w);
//				glfwFocusWindow(w);
//			}
//		}
    }
	
	void RenderWindow(Window & w)
	{
//		if (!w.m_focused)
//			return;
		
		int iconified = glfwGetWindowAttrib(w.m_window, GLFW_ICONIFIED);
		if (iconified)
			return;
		
		glfwMakeContextCurrent(w.m_window);
		g_renderContext.window = w.m_window;
		
		double mx, my;
		glfwGetCursorPos(w.m_window, &mx, &my);
		Input::m_mousePosition.x = float(mx);
		Input::m_mousePosition.y = float(my);
		
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(w.m_window, &fbWidth, &fbHeight);
		int width, height;
		glfwGetWindowSize(w.m_window, &width, &height);
		
		g_renderContext.width = width;
		g_renderContext.height = height;
		
		g_centerScreen.reset();
		
		GLNVGcontext * gl = (GLNVGcontext *)nvgInternalParams(vg)->userPtr;
		if (w.m_buffers == nullptr)
		{
			w.m_buffers = glnvg__createVertexBuffers(gl);
		}
		gl->buffers = w.m_buffers;
		
		glViewport(0, 0, fbWidth, fbHeight);
		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		float ratio = float(fbWidth) / width;
		nvgBeginFrame(vg, width, height, ratio);
		
		for (auto& tab : w.m_tabs)
		{
//			BeginTab(tab->m_title.c_str(), &tab->m_widthOrHeight, tab->m_position);
			BeginTab(tab);
			if (tab->m_renderFunction != nullptr)
				tab->m_renderFunction();
			EndTab();
		}
		
		nvgEndFrame(vg);
		glCheckError();
		glfwSwapBuffers(w.m_window);
	}
	
	void RenderTabs()
	{
		if (g_renderContext.isAllWindowInvisiable)
			return;
		RenderWindow(g_mainWindow);
		for (auto& w : g_floatingWindows)
		{
			RenderWindow(*w);
		}
	}
	
	void BeginTab(Tab * tab)
	{
		g_currentTab.reset();
		auto tabPosition = tab->m_position;
		const char * title = tab->m_title.c_str();
		
		bool mouseHoverResizeBar = false;
		
		if (tabPosition == TabPosition::Floating)
		{
			auto& r = g_currentTab.rect;
//			widthOrHeight = g_renderContext.width;
			g_centerScreen.addTab(tabPosition, tab->m_size, &r);
			drawWindow(vg, title, r.x, r.y, r.width, r.height);
			g_currentTab.y_filled += 20;
		}
		else
		{
			auto& r = g_currentTab.rect;
			g_centerScreen.addTab(tabPosition, tab->m_size, &r);
//			g_currentTab.y_filled = 0;
			
			constexpr int margin = 5;
			const int x = Input::m_mousePosition.x;
			const int y = Input::m_mousePosition.y;
			if (tabPosition == TabPosition::Left)
			{
				if (tab->m_resizing)
				{
					if (Input::GetMouseButton(MouseButtonCode::Left))
					{
						int right = r.x + r.width;
						r.width += x - right;
						r.width = std::max(r.width, tab->m_minimalSize);
						tab->m_size = r.width;
					}
					else
					{
						tab->m_resizing = false;
					}
				}
				
				if (x > r.x+r.width-margin &&
					x < r.x+r.width+margin)
				{
					SetCursorType(CursorType::HResize);
					mouseHoverResizeBar = true;
				}
			}
			else if (tabPosition == TabPosition::Right)
			{
				if (tab->m_resizing)
				{
					if (Input::GetMouseButton(MouseButtonCode::Left))
					{
						int right = r.x + r.width;
						r.width += r.x - x;
						r.width = std::max(r.width, tab->m_minimalSize);
						r.x = right - r.width;
						tab->m_size = r.width;
					}
					else
					{
						tab->m_resizing = false;
					}
				}
				
				if (x > r.x-margin && x < r.x+margin)
				{
					SetCursorType(CursorType::HResize);
					mouseHoverResizeBar = true;
				}
			}
			else if (tabPosition == TabPosition::Top)
			{
				if (tab->m_resizing)
				{
					if (Input::GetMouseButton(MouseButtonCode::Left))
					{
						int bottom = r.y + r.height;
						r.height += y - bottom;
						r.height = std::max(r.height, tab->m_minimalSize);
						tab->m_size = r.height;
					}
					else
					{
						tab->m_resizing = false;
					}
				}

				if (y > r.y+r.height-margin &&
					y < r.y+r.height+margin)
				{
					SetCursorType(CursorType::VResize);
					mouseHoverResizeBar = true;
				}
			}
			else if (tabPosition == TabPosition::Bottom)
			{
				if (tab->m_resizing)
				{
					if (Input::GetMouseButton(MouseButtonCode::Left))
					{
//						int bottom = r.y + r.height;
//						r.height += y - bottom;
//						r.height = std::max(r.height, tab->m_minimalSize);
//						tab->m_size = r.height;
						
						int old_bottom = r.y + r.height;
						r.height += r.y - y;
						r.height = std::max(r.height, tab->m_minimalSize);
						r.y = old_bottom - r.height;
						tab->m_size = r.height;
					}
					else
					{
						tab->m_resizing = false;
					}
				}
				
				if (y > r.y-margin && y < r.y+margin)
				{
					SetCursorType(CursorType::VResize);
					mouseHoverResizeBar = true;
				}
			}
			
			if (mouseHoverResizeBar && Input::GetMouseButtonDown(MouseButtonCode::Left))
			{
				tab->m_resizing = true;
			}
			
			drawWindow(vg, title, r.x, r.y, r.width, r.height);
			g_currentTab.y_filled += 20;
		}
    }
	
	void EndTab()
	{
	}
    
    bool Button(const char* text)
	{
        int x, y, w, h;
        g_currentTab.nextCellOrigin(&x, &y);
        w = g_currentTab.get_avaliable_width();
        h = g_currentTab.y_cell_height;
		
        bool clicked = false;
//        int id = g_currentTab.get_current_cell_id();
        bool mouse_inside = mouseInRegion(x, y, w, h);
		NVGcolor colorTop = g_theme.mButtonGradientTopUnfocused;
		NVGcolor colorBot = g_theme.mButtonGradientBotUnfocused;
		if (mouse_inside)
		{
			colorTop = g_theme.mButtonGradientTopFocused;
			colorBot = g_theme.mButtonGradientBotFocused;
			if (Input::GetMouseButtonUp(MouseButtonCode::Left)) {
				clicked = true;
			} else if (Input::GetMouseButton(MouseButtonCode::Left)) {
				colorTop = g_theme.mButtonGradientTopPushed;
				colorBot = g_theme.mButtonGradientBotPushed;
			}
		}
		drawButton(vg, 0, text, x, y, w, h, colorTop, colorBot);

        g_currentTab.add_cell(h);
        return clicked;
    }
    
    void Label(const char* text, GUIAlignment alignment) {
        int x, y, w, h;
        g_currentTab.nextCellOrigin(&x, &y);
        w = g_currentTab.get_avaliable_width();
        h = g_currentTab.y_cell_height;
		drawLabel(vg, text, x, y, w, h);
		
        g_currentTab.add_cell(h);
    }
	
	bool CheckBox(const char * label, bool* v)
	{
		int x, y, w, h;
		g_currentTab.nextCellOrigin(&x, &y);
		w = g_currentTab.get_avaliable_width();
		h = g_currentTab.y_cell_height;
		drawCheckBox(vg, label, x, y, w, h);
		g_currentTab.add_cell(h);
		return false;
	}
	
	bool InputText(const char* label, std::string& text)
	{
		int x, y, w, h;
		g_currentTab.nextCellOrigin(&x, &y);
		w = g_currentTab.get_avaliable_width();
		h = g_currentTab.y_cell_height;
		int label_width = w / 2 - g_currentTab.x_margin_left;
		drawLabel(vg, label, x, y, label_width, h);
		x += label_width + g_currentTab.x_margin_left;
		w -= label_width + g_currentTab.x_margin_left;
		drawEditBox(vg, text.c_str(), x, y, w, h);
		
		if (mouseInRegion(x, y, w, h)) {
			SetCursorType(CursorType::IBeam);
			if (Input::GetMouseButtonDown(MouseButtonCode::Left)) {
//				std::cout << "here" << std::endl;
				Input::m_inputMode = true;
			}
		}
		
		g_currentTab.add_cell(h);
		return false;
	}
	
	bool Slider(const char* label, float *value, float minValue, float maxValue)
	{
		int x, y, w, h;
		g_currentTab.nextCellOrigin(&x, &y);
		w = g_currentTab.get_avaliable_width();
		h = g_currentTab.y_cell_height;
		int label_width = w/2 - g_currentTab.x_margin_left;
		int box_width = 40;
		int slider_width = w - label_width - box_width - g_currentTab.x_margin_left*2;
		
		drawLabel(vg, label, x, y, label_width, h);
		x += label_width + g_currentTab.x_margin_left;
		float pos = (*value - minValue) / (maxValue - minValue);
		drawSlider(vg, pos, x, y, slider_width, h);
		
		x += slider_width + g_currentTab.x_margin_left;
//		assert(label_width + box_width + slider_width + g_currentTab.x_margin_left*2 == w);
		auto value_str = StringCast(*value);
		drawEditBox(vg, value_str.c_str(), x, y, box_width, h);
		
		g_currentTab.add_cell(h);
		return false;
	}
}
