#include "RFGUI.hpp"

#include "nanovg.h"
//#define NANOVG_GL3
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include <vector>
#include <stack>
#include <cassert>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>

#define ICON_SEARCH 0x1F50D
#define ICON_CIRCLED_CROSS 0x2716
#define ICON_CHEVRON_RIGHT 0xE75E
#define ICON_CHECK 0x2713
#define ICON_LOGIN 0xE740
#define ICON_TRASH 0xE729

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
	
	struct WindowState
	{
		GLFWwindow *m_handle = nullptr;
		
//		Window(int w, int h, const char* title)
//		{
//			m_handle = glfwCreateWindow(w, h, title, nullptr, nullptr);
//		}
	};
	
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
		
		void addTab(TabPosition tabPosition, int widthOrHeight, Rect * xywh)
		{
			if (tabPosition == TabPosition::Left) {
				assert(widthOrHeight <= w);
				xywh->x = x;
				xywh->y = y;
				xywh->width = widthOrHeight;
				xywh->height = h;
				x += widthOrHeight;
				w -= widthOrHeight;
			} else if (tabPosition == TabPosition::Right) {
				assert(widthOrHeight <= w);
				xywh->x = x+w-widthOrHeight;
				xywh->y = y;
				xywh->width = widthOrHeight;
				xywh->height = h;
				w -= widthOrHeight;
			} else if (tabPosition == TabPosition::Top) {
				assert(widthOrHeight <= h);
				xywh->x = x;
				xywh->y = y;
				xywh->width = w;
				xywh->height = widthOrHeight;
				y += widthOrHeight;
				h -= widthOrHeight;
			} else {	// Bottom
				assert(widthOrHeight <= h);
				xywh->x = x;
				xywh->y = y+h-widthOrHeight;
				xywh->width = w;
				xywh->height = widthOrHeight;
				h -= widthOrHeight;
			}
		}
	};
	
	static CenterScreenState g_centerScreen;
	
	struct TabState
	{
		int x;
		int y;
		int width;
		int height;
		const int x_margin_left = 10;
		int x_margin_right = 10; // or 18
		const int y_margin = 5;
		const int y_cell_height = 20;
		int y_start = 0;
		int y_filled = 0;
		int cell_count = 0;
		int hover_cell_id = -1;
		int clicked_cell_id = -1;
		
		void reset() {
			y_filled = 0;
			cell_count = 0;
		}
		
		void get_origin(int *px, int *py) {
			*px = x + x_margin_left;
			*py = y + y_filled + y_margin;
		}
		
		// top left of the next cell
		void nextCellOrigin(int &px, int &py) {
			px = x + x_margin_left;
			py = y + y_filled + y_margin;
		}
		
		int get_avaliable_width() {
			return width - x_margin_left - x_margin_right;
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
	void SetCursorType(CursorType type) {
		if (g_cursorType == type) {
			return;
		}
		g_cursorType = type;
		glfwSetCursor(g_renderContext.window, g_cursors[static_cast<int>(type)]);
	}
	
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
	
    void window_size_callback(GLFWwindow* window, int width, int height)
    {
        g_renderContext.width = width;
        g_renderContext.height = height;
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
		std::cout << "glfw_window_focus_callback" << std::endl;
		if (focused)
		{
			glfwMakeContextCurrent(window);
			g_renderContext.window = window;
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
//		WindowManager::Instance().m_mainWindow = window;
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
	
	void MakeCurrent(GLFWwindow * window)
	{
		if (g_renderContext.window == window)
			return;
		
		if (g_renderContext.window != nullptr)
			WindowManager::Instance().PushWindow(g_renderContext.window);
		
		glfwMakeContextCurrent(window);
		
		glfwSetWindowSizeCallback(window, RFGUI::window_size_callback);
		glfwSetKeyCallback(window, RFGUI::key_callback);
		glfwSetMouseButtonCallback(window, RFGUI::mouse_button_callback);
		glfwSetCharCallback(window, RFGUI::glfw_char_callback);
		glfwSetWindowSizeCallback(window, glfw_window_resize_callback);
		glfwSetWindowFocusCallback(window, glfw_window_focus_callback);
		int width = 0;
		int height = 0;
		glfwGetWindowSize(window, &width, &height);
		glfw_window_resize_callback(window, width, height);
		g_renderContext.window = window;
	}
	
	static TabState g_currentTab;
    
    void Begin()
	{
		double mx, my;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		
		glfwGetCursorPos(g_renderContext.window, &mx, &my);
		glfwGetWindowSize(g_renderContext.window, &winWidth, &winHeight);
		glfwGetFramebufferSize(g_renderContext.window, &fbWidth, &fbHeight);
		
		
		g_renderContext.width = winWidth;
		g_renderContext.height = winHeight;
		glViewport(0, 0, fbWidth, fbHeight);
		
		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		// Calculate pixel ration for hi-dpi devices.
		float pxRatio = (float)fbWidth / (float)winWidth;
		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
		
		Input::m_mousePosition.x = float(mx);
		Input::m_mousePosition.y = float(my);
		
		SetCursorType(CursorType::Arrow);
		
		g_centerScreen.reset();
//		glfwFocusWindow(g_renderContext.window);
    }
    
    void End()
	{
        nvgEndFrame(vg);
		Input::Update();
		glfwSwapBuffers(g_renderContext.window);
		glfwPollEvents();
		
		if (g_renderContext.window != WindowManager::Instance().mainWindow() && glfwWindowShouldClose(g_renderContext.window))
		{
			glfwHideWindow(g_renderContext.window);
			auto w = WindowManager::Instance().PopWindow();
			if (w != nullptr)
			{
				g_renderContext.window = w;
				glfwMakeContextCurrent(w);
			}
		}
    }
	
    void BeginTab(const char* title, int widthOrHeight, TabPosition tabPosition)
	{
		Rect r;
		g_centerScreen.addTab(tabPosition, widthOrHeight, &r);
		g_currentTab.reset();
//		auto tab = new TabState;
//		g_tabs.push_back(tab);
//		g_currentTab = tab;
        g_currentTab.x = r.x;
		g_currentTab.y = r.y;
        g_currentTab.y_filled = 0;
        g_currentTab.width = r.width;
		g_currentTab.height = r.height;
		
		drawWindow(vg, title, r.x, r.y, r.width, r.height);
		g_currentTab.y_filled += 20;
    }
	
	void EndTab()
	{
		
	}
    
    bool Button(const char* text) {
        int x, y, w, h;
        g_currentTab.get_origin(&x, &y);
        w = g_currentTab.get_avaliable_width();
        h = g_currentTab.y_cell_height;
		
        bool clicked = false;
//        int id = g_currentTab.get_current_cell_id();
        bool mouse_inside = mouseInRegion(x, y, w, h);
//		drawButton(vg, ICON_TRASH, "Delete", x, y, 160, 28, nvgRGBA(128,16,8,255));
		NVGcolor colorTop = g_theme.mButtonGradientTopUnfocused;
		NVGcolor colorBot = g_theme.mButtonGradientBotUnfocused;
		if (mouse_inside) {
			colorTop = g_theme.mButtonGradientTopFocused;
			colorBot = g_theme.mButtonGradientBotFocused;
			if (Input::GetMouseButtonUp(MouseButtonCode::Left)) {
				clicked = true;
			} else if (Input::GetMouseButton(MouseButtonCode::Left)) {
				colorTop = g_theme.mButtonGradientTopPushed;
				colorBot = g_theme.mButtonGradientBotPushed;
			}
		} else {
//			color = nvgRGBA(0, 0, 0, 0);
		}
		drawButton(vg, 0, text, x, y, w, h, colorTop, colorBot);

        g_currentTab.add_cell(h);
        return clicked;
    }
    
    void Label(const char* text, GUIAlignment alignment) {
        int x, y, w, h;
        g_currentTab.get_origin(&x, &y);
        w = g_currentTab.get_avaliable_width();
        h = g_currentTab.y_cell_height;
		drawLabel(vg, text, x, y, w, h);
		
        g_currentTab.add_cell(h);
    }
	
	bool CheckBox(const char * label, bool* v)
	{
		int x, y, w, h;
		g_currentTab.get_origin(&x, &y);
		w = g_currentTab.get_avaliable_width();
		h = g_currentTab.y_cell_height;
		drawCheckBox(vg, label, x, y, w, h);
		g_currentTab.add_cell(h);
		return false;
	}
	
	bool InputText(const char* label, std::string& text)
	{
		int x, y, w, h;
		g_currentTab.get_origin(&x, &y);
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
				std::cout << "here" << std::endl;
				Input::m_inputMode = true;
			}
		}
		
		g_currentTab.add_cell(h);
		return false;
	}
	
	bool Slider(const char* label, float *value, float minValue, float maxValue)
	{
		int x, y, w, h;
		g_currentTab.get_origin(&x, &y);
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
