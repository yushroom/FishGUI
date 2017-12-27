#ifdef _WIN32
//#define GLEW_STATIC
#include <gl/glew.h>
#else
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"


#include "FishGUI.hpp"
#include "Input.hpp"
#include "Window.hpp"

#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

#define ICON_SEARCH 0x1F50D
#define ICON_CIRCLED_CROSS 0x2716
#define ICON_CHEVRON_RIGHT 0xE75E
#define ICON_CHECK 0x2713
#define ICON_LOGIN 0xE740
#define ICON_TRASH 0xE729

constexpr NVGcolor Color(int intensity, int alpha)
{
	return NVGcolor{{{intensity/255.0f, intensity/255.0f, intensity/255.0f, alpha/255.0f}}};
}

struct Theme
{
	int mStandardFontSize                 = 16;
	int mButtonFontSize                   = 20;
	int mTextBoxFontSize                  = 20;
	int mWindowCornerRadius               = 2;
	int mWindowHeaderHeight               = 18;
	int mWindowDropShadowSize             = 10;
	int mButtonCornerRadius               = 2;
	float mTabBorderWidth                 = 0.75f;
	int mTabInnerMargin                   = 5;
	int mTabMinButtonWidth                = 20;
	int mTabMaxButtonWidth                = 160;
	int mTabControlWidth                  = 20;
	int mTabButtonHorizontalPadding       = 10;
	int mTabButtonVerticalPadding         = 2;
	
	NVGcolor mDropShadow                       = Color(0, 128);
	NVGcolor mTransparent                      = Color(0, 0);
	NVGcolor mBorderDark                       = Color(29, 255);
	NVGcolor mBorderLight                      = Color(133, 255);
	NVGcolor mBorderMedium                     = Color(35, 255);
//	NVGcolor mTextColor                        = Color(255, 160);
	NVGcolor mTextColor = Color(0, 255);
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
	NVGcolor mWindowBackgroundColor = Color(162, 255);
//	NVGcolor mWindowFocusedHeaderColor = Color(230, 255);
	NVGcolor mTabContentBackgroundColor = Color(194, 255);
	
	float mTabHeaderFontSize = 16.0f;
	int mTabHeaderCellWidth = 80;
	NVGcolor mTabHeaderActiveColor = mTabContentBackgroundColor;
	
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

static Theme theme;

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

void DrawTabButton(NVGcontext* ctx, const char* text, int x, int y, int w, int h, bool active)
{
	nvgSave(ctx);
	nvgIntersectScissor(ctx, x, y, w, h);
	
	if (active)
	{
		nvgBeginPath(ctx);
		nvgRoundedRect(ctx, x, y, w, h+theme.mButtonCornerRadius, theme.mButtonCornerRadius);
		nvgFillColor(ctx, theme.mTabHeaderActiveColor);
		nvgFill(ctx);
	}
	nvgResetScissor(ctx);
	nvgRestore(ctx);
	
	// Draw the text with some padding
	int textX = x + theme.mTabButtonHorizontalPadding;
	int textY = y + theme.mTabButtonVerticalPadding;
	NVGcolor textColor = theme.mTextColor;
	nvgBeginPath(ctx);
	nvgFillColor(ctx, textColor);
	nvgFontSize(ctx, theme.mTabHeaderFontSize);
	nvgFontFace(ctx, "sans");
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgText(ctx, textX, textY, text, nullptr);
}



//void DrawDock(NVGcontext* ctx, float x, float y, float w, float h)
//{
//	float cornerRadius = 3.0f;
////	NVGpaint shadowPaint;
//	NVGpaint headerPaint;
//
//	nvgSave(ctx);
////	nvgClearState(vg);
//
//	nvgBeginPath(ctx);
//	nvgRect(ctx, x, y, w, h);
//	nvgFillColor(ctx, theme.mWindowBackgroundColor);
//	nvgFill(ctx);
//	
//	nvgBeginPath(ctx);
//	nvgFillColor(ctx, theme.mWindowBackgroundColor);
//	nvgRect(ctx, x, y, w, theme.mWindowHeaderHeight);
//	nvgFill(ctx);
//
//	nvgRestore(ctx);
//}


void drawSearchBox(NVGcontext* vg, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = h / 2 - 1;

	// Edit
	bg = nvgBoxGradient(vg, x, y + 1.5f, w, h, h / 2, 5, nvgRGBA(0, 0, 0, 16), nvgRGBA(0, 0, 0, 92));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x, y, w, h, cornerRadius);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	/*	nvgBeginPath(vg);
	nvgRoundedRect(vg, x+0.5f,y+0.5f, w-1,h-1, cornerRadius-0.5f);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,48));
	nvgStroke(vg);*/

	nvgFontSize(vg, h*1.3f);
	nvgFontFace(vg, "icons");
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 64));
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(vg, x + h * 0.55f, y + h * 0.55f, cpToUTF8(ICON_SEARCH, icon), NULL);

	nvgFontSize(vg, 20.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 32));

	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgText(vg, x + h * 1.05f, y + h * 0.5f, text, NULL);

	nvgFontSize(vg, h*1.3f);
	nvgFontFace(vg, "icons");
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 32));
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(vg, x + w - h * 0.55f, y + h * 0.55f, cpToUTF8(ICON_CIRCLED_CROSS, icon), NULL);
}

void drawDropDown(NVGcontext* vg, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = 4.0f;

	bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(255, 255, 255, 16), nvgRGBA(0, 0, 0, 16));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x + 1, y + 1, w - 2, h - 2, cornerRadius - 1);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgRoundedRect(vg, x + 0.5f, y + 0.5f, w - 1, h - 1, cornerRadius - 0.5f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 48));
	nvgStroke(vg);

	nvgFontSize(vg, 20.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 160));
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgText(vg, x + h * 0.3f, y + h * 0.5f, text, NULL);

	nvgFontSize(vg, h*1.3f);
	nvgFontFace(vg, "icons");
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 64));
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(vg, x + w - h * 0.5f, y + h * 0.5f, cpToUTF8(ICON_CHEVRON_RIGHT, icon), NULL);
}

void drawLabel(NVGcontext* ctx, const char* text, float x, float y, float w, float h)
{
	NVG_NOTUSED(w);

	nvgFontSize(ctx, 18.0f);
	nvgFontFace(ctx, "sans");
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 128));

	//nvgBeginPath(ctx);
	//nvgRect(ctx, x, y, w, h);
	//nvgFill(ctx);

	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgSave(ctx);
	nvgIntersectScissor(ctx, x, y, w, h);
	nvgText(ctx, x, y + h * 0.5f, text, nullptr);
	nvgRestore(ctx);
}

void drawEditBoxBase(NVGcontext* vg, float x, float y, float w, float h)
{
	NVGpaint bg;
	// Edit
	bg = nvgBoxGradient(vg, x + 1, y + 1 + 1.5f, w - 2, h - 2, 3, 4, nvgRGBA(255, 255, 255, 32), nvgRGBA(32, 32, 32, 32));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x + 1, y + 1, w - 2, h - 2, 4 - 1);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgRoundedRect(vg, x + 0.5f, y + 0.5f, w - 1, h - 1, 4 - 0.5f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 48));
	nvgStroke(vg);
}

void drawEditBox(NVGcontext* ctx, const char* text, float x, float y, float w, float h)
{

	drawEditBoxBase(ctx, x, y, w, h);

	nvgSave(ctx);
	nvgIntersectScissor(ctx, x, y, w, h);

	nvgFontSize(ctx, 16.0f);
	nvgFontFace(ctx, "sans");
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 64));
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgText(ctx, x + h * 0.3f, y + h * 0.5f, text, NULL);

	nvgRestore(ctx);
}

void drawCheckBox(NVGcontext* vg, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	NVG_NOTUSED(w);

	nvgFontSize(vg, 18.0f);
	nvgFontFace(vg, "sans");
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 160));

	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgText(vg, x + 28, y + h * 0.5f, text, NULL);

	bg = nvgBoxGradient(vg, x + 1, y + (int)(h*0.5f) - 9 + 1, 18, 18, 3, 3, nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 92));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x + 1, y + (int)(h*0.5f) - 9, 18, 18, 3);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	nvgFontSize(vg, 40);
	nvgFontFace(vg, "icons");
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 128));
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(vg, x + 9 + 2, y + h * 0.5f, cpToUTF8(ICON_CHECK, icon), NULL);
}

void drawButton(NVGcontext* ctx, int preicon, const char* text, float x, float y, float w, float h, NVGcolor colTop, NVGcolor colBot)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = 4.0f;
	float tw = 0, iw = 0;

	//	bg = nvgLinearGradient(vg, x,y,x,y+h, nvgRGBA(255,255,255,isBlack(col)?16:32), nvgRGBA(0,0,0,isBlack(col)?16:32));
	bg = nvgLinearGradient(ctx, x, y, x, y + h, colTop, colBot);
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x + 1, y + 1, w - 2, h - 2, cornerRadius - 1);
	//	if (!isBlack(col)) {
	//		nvgFillColor(vg, col);
	//		nvgFill(vg);
	//	}
	nvgFillPaint(ctx, bg);
	nvgFill(ctx);

	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x + 0.5f, y + 0.5f, w - 1, h - 1, cornerRadius - 0.5f);
	nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 48));
	nvgStroke(ctx);

	nvgFontSize(ctx, 20.0f);
	nvgFontFace(ctx, "sans-bold");
	tw = nvgTextBounds(ctx, 0, 0, text, NULL, NULL);
	if (preicon != 0) {
		nvgFontSize(ctx, h*1.3f);
		nvgFontFace(ctx, "icons");
		iw = nvgTextBounds(ctx, 0, 0, cpToUTF8(preicon, icon), NULL, NULL);
		iw += h * 0.15f;
	}

	if (preicon != 0) {
		nvgFontSize(ctx, h*1.3f);
		nvgFontFace(ctx, "icons");
		nvgFillColor(ctx, nvgRGBA(255, 255, 255, 96));
		nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgText(ctx, x + w * 0.5f - tw * 0.5f - iw * 0.75f, y + h * 0.5f, cpToUTF8(preicon, icon), NULL);
	}

	nvgSave(ctx);
	nvgIntersectScissor(ctx, x, y, w, h);

	nvgFontSize(ctx, 20.0f);
	nvgFontFace(ctx, "sans-bold");
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgFillColor(ctx, nvgRGBA(0, 0, 0, 160));
	nvgText(ctx, x + w * 0.5f - tw * 0.5f + iw * 0.25f, y + h * 0.5f - 1, text, NULL);
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 160));
	nvgText(ctx, x + w * 0.5f - tw * 0.5f + iw * 0.25f, y + h * 0.5f, text, NULL);

	nvgRestore(ctx);
}

void drawSlider(NVGcontext* vg, float pos, float x, float y, float w, float h)
{
	NVGpaint bg, knob;
	float cy = y + (int)(h*0.5f);
	float kr = (int)(h*0.25f);

	nvgSave(vg);
	//	nvgClearState(vg);

	// Slot
	bg = nvgBoxGradient(vg, x, cy - 2 + 1, w, 4, 2, 2, nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 128));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x, cy - 2, w, 4, 2);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	// Knob Shadow
	bg = nvgRadialGradient(vg, x + (int)(pos*w), cy + 1, kr - 3, kr + 3, nvgRGBA(0, 0, 0, 64), nvgRGBA(0, 0, 0, 0));
	nvgBeginPath(vg);
	nvgRect(vg, x + (int)(pos*w) - kr - 5, cy - kr - 5, kr * 2 + 5 + 5, kr * 2 + 5 + 5 + 3);
	nvgCircle(vg, x + (int)(pos*w), cy, kr);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	// Knob
	knob = nvgLinearGradient(vg, x, cy - kr, x, cy + kr, nvgRGBA(255, 255, 255, 16), nvgRGBA(0, 0, 0, 16));
	nvgBeginPath(vg);
	nvgCircle(vg, x + (int)(pos*w), cy, kr - 1);
	nvgFillColor(vg, nvgRGBA(40, 43, 48, 255));
	nvgFill(vg);
	nvgFillPaint(vg, knob);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, x + (int)(pos*w), cy, kr - 0.5f);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 92));
	nvgStroke(vg);

	nvgRestore(vg);
}

//namespace FishGUI
//{
//	constexpr int WIDTH = 60;
//	constexpr int HEIGHT = 20;
//
//	enum class WidgetType
//	{
//		ELabel,
//		EButton,
//		ECheckBox,
//		ETextEdit,
//		EDropDown,
//		ESlider,
//	};
//
//	template<WidgetType T>
//	Size PreferredSize()
//	{
//		constexpr int width = 60;
//		constexpr int height = 30;
//		return Size{ width, height };
//	}
//
//	template<WidgetType T>
//	Size MinSize()
//	{
//		constexpr int width = 40;
//		constexpr int height = 20;
//		return Size{ width, height };
//	}
//
//	template<WidgetType T>
//	Rect GetRenderRect(Alignment v_alignment, Alignment h_alignment, const Rect& rect)
//	{
//		Rect r = rect;
//		Size pSize = PreferredSize<T>();
//		Size minSize = MinSize<T>();
//
//		// horizontally alignment
//		if (h_alignment == Alignment::Minimum)
//		{
//			r.width = minSize.width;
//		}
//		else if (h_alignment == Alignment::Middle)
//		{
//			r.width = pSize.width;
//			if (r.width < rect.width)
//			{
//				r.width = rect.width;
//			}
//			else
//			{
//				r.x += std::floorf((rect.width - r.width) / 2.0f);
//			}
//		}
//		else if (h_alignment == Alignment::Maximum)
//		{
//			// r.width = rect.width;
//		}
//		else if (h_alignment == Alignment::Fill)
//		{
//			r.width = size.width;
//			if (r.width < rect.width)
//			{
//				r.width = rect.width;
//			}
//		}
//
//		// vertically alignment
//		if (v_alignment == Alignment::Minimum)
//		{
//			r.height = minSize.height;
//		}
//		else if (v_alignment == Alignment::Middle)
//		{
//			r.height = pSize.height;
//			if (r.height < rect.height)
//			{
//				r.height = rect.height;
//			}
//			else
//			{
//				r.y += std::floorf((rect.height - r.height) / 2.0f);
//			}
//		}
//		else if (v_alignment == Alignment::Maximum)
//		{
//			// r.height = rect.height;
//		}
//		else if (v_alignment == Alignment::Fill)
//		{
//			r.height = size.height;
//			if (r.height < rect.height)
//			{
//				r.height = rect.height;
//			}
//		}
//		//widget.Draw(r);
//		return r;
//	}
//
//	class Context
//	{
//	public:
//		std::stack<Alignment> m_hAlignments;
//		std::stack<Alignment> m_vAlignments;
//		std::stack<Orientation> m_orientations;
//
//		Vector2 m_topLeft = Vector2{ 0, 0 };
//		Vector2 m_anchor = Vector2{ 0, 0 };
//		Size m_sizeOfCurrentCell;
//
//		Vector2 PosInScreen(const Vector2& localPos)
//		{
//			return localPos + m_topLeft;
//		}
//
//		Orientation orientation() { return m_orientations.top(); }
//
//		void NewCell()
//		{
//			if (orientation() == Orientation::Vertical)
//			{
//				if (m_anchor.x != 0)
//				{
//					m_anchor.x = 0;
//					m_anchor.y += m_sizeOfCurrentCell.height;
//				}
//			}
//			else
//			{
//				if (m_anchor.y != 0)
//				{
//					m_anchor.y = 0;
//					m_anchor.x += m_sizeOfCurrentCell.width;
//				}
//			}
//
//		}
//	};
//
//	Rect GetRect()
//	{
//		return { 0, 0, 100, 20 };
//	}
//
//	bool Button(Context& ctx, const char* text)
//	{
//		Rect r = GetRect();
//		r = GetRenderRect<WidgetType::EButton>(Alignment::Fill, Alignment::Fill, r);
//		bool clicked = false;
//		//		int id = g.currentTab.get_current_cell_id();
//		bool mouse_inside = MouseInRect(r);
//		NVGcolor colorTop = g.theme.mButtonGradientTopUnfocused;
//		NVGcolor colorBot = g.theme.mButtonGradientBotUnfocused;
//		if (mouse_inside)
//		{
//			colorTop = g.theme.mButtonGradientTopFocused;
//			colorBot = g.theme.mButtonGradientBotFocused;
//			if (IsMouseButtonUp(MouseButton::Left)) {
//				clicked = true;
//			}
//			else if (IsMouseButtonPressed(MouseButton::Left)) {
//				colorTop = g.theme.mButtonGradientTopPushed;
//				colorBot = g.theme.mButtonGradientBotPushed;
//			}
//		}
//		drawButton(g.nvgContext, 0, text, r.x, r.y, r.width, r.height, colorTop, colorBot);
//	}
//}

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
			if (input.GetMouseButtonDown(MouseButton::Left) && Input::GetInstance().MouseInRect(r))
			{
				newActivateId = i;
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

		nvgRestore(ctx);

		Widget::Draw();
	}
	
	void Window::Draw()
	{
		int width = m_size.width;
		int height = m_size.height;
		int fbWidth = m_size.width * 2;
		int fbHeight = m_size.height * 2;
		glfwGetWindowSize(m_glfwWindow, &width, &height);
		glfwGetFramebufferSize(m_glfwWindow, &fbWidth, &fbHeight);
		m_size.width = width;
		m_size.height = height;

		m_rect.width = width;
		m_rect.height = height;
		
		GLNVGcontext * gl = (GLNVGcontext *)nvgInternalParams(m_context->m_nvgContext)->userPtr;
		if (m_buffers == nullptr)
		{
			m_buffers = glnvg__createVertexBuffers(gl);
		}
		gl->buffers = m_buffers;
		Rect remains{ 0, 0, m_size.width, m_size.height };
		
		glViewport(0, 0, fbWidth, fbHeight);
		float bck = 162 / 255.0f;
		glClearColor(bck, bck, bck, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		float ratio = float(fbWidth) / width;
		nvgBeginFrame(m_context->m_nvgContext, width, height, ratio);

		Widget::Draw();
		
		nvgEndFrame(m_context->m_nvgContext);
		glfwSwapBuffers(m_glfwWindow);
		//std::this_thread::sleep_for(33.3ms);
	}

	void MainWindow::Draw()
	{
		int width = m_size.width;
		int height = m_size.height;
		int fbWidth = m_size.width * 2;
		int fbHeight = m_size.height * 2;
		glfwGetWindowSize(m_glfwWindow, &width, &height);
		glfwGetFramebufferSize(m_glfwWindow, &fbWidth, &fbHeight);
		m_size.width = width;
		m_size.height = height;

		m_rect.width = width;
		m_rect.height = height;

		GLNVGcontext * gl = (GLNVGcontext *)nvgInternalParams(m_context->m_nvgContext)->userPtr;
		if (m_buffers == nullptr)
		{
			m_buffers = glnvg__createVertexBuffers(gl);
		}
		gl->buffers = m_buffers;
		Rect remains{ 0, 0, m_size.width, m_size.height };

		glViewport(0, 0, fbWidth, fbHeight);
		float bck = 162 / 255.0f;
		glClearColor(bck, bck, bck, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		float ratio = float(fbWidth) / width;
		nvgBeginFrame(m_context->m_nvgContext, width, height, ratio);

		m_toolBar.SetRect(0, 0, width, m_toolBar.GetHeight());
		m_toolBar.Draw();
		m_statusBar.SetRect(0, height - m_statusBar.GetHeight(), width, m_statusBar.GetHeight());
		m_statusBar.Draw();

		m_rect.height -= m_toolBar.GetHeight() + m_statusBar.GetHeight();
		m_rect.y += m_toolBar.GetHeight();
		Widget::Draw();
		m_rect.height += m_toolBar.GetHeight() + m_statusBar.GetHeight();
		m_rect.y -= m_toolBar.GetHeight();

		nvgEndFrame(m_context->m_nvgContext);
		glfwSwapBuffers(m_glfwWindow);
		//std::this_thread::sleep_for(33.3ms);
	}
	
	static void GlfwErrorCallback(int error, const char* description)
	{
		fputs(description, stderr);
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
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
		
		auto window = new MainWindow(&context, "Fish GUI", 800, 600);
		WindowManager::GetInstance().m_windows.push_back(window);
		glfwMakeContextCurrent(window->GetGLFWWindow());
		glfwSwapInterval(1);
		
#ifdef _WIN32
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
		auto win = WindowManager::GetInstance().GetMainWindow();
		
		while (!glfwWindowShouldClose(win->GetGLFWWindow()))
		{
			BeforeFrame();
			auto& input = Input::GetInstance();
			double mx, my;
			glfwGetCursorPos(win->GetGLFWWindow(), &mx, &my);
			input.m_mousePosition.x = int(mx);
			input.m_mousePosition.y = int(my);
			win->Draw();
			input.Update();
			AfterFrame();
			glfwPollEvents();
		}
		
		glfwDestroyWindow(win->GetGLFWWindow());
		glfwTerminate();
	}
}
