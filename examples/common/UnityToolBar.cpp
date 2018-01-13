#include "UnityToolBar.hpp"
#include <FishGUI/FishGUI.hpp>

#include <FishGUI/Draw.hpp>
#include <FishGUI/Theme.hpp>
#include "nanovg_gl.h"
#include <FishGUI/Utils.hpp>
#include <FishGUI/Input.hpp>

// code point of ui-icon in icomoon.ttf
constexpr int ICO_HAND = 0xe900;
constexpr int ICO_MOVE = 0xe902;
constexpr int ICO_ROTATE = 0xe984;
constexpr int ICO_SCALE = 0xe989;

constexpr int ICO_START = 0xe903;
constexpr int ICO_PAUSE = 0xe904;
constexpr int ICO_STOP = 0xe905;
constexpr int ICO_NEXT = 0xe906;

using namespace FishGUI;

struct SegmentedButtons
{
	static void Begin(const FishGUI::Rect& r, int segmentCount)
	{
		g_segCount = segmentCount;
		g_rect = r;
		
		constexpr float cornerRadius = 4.0f;
		int x = r.x;
		int y = r.y;
		int w = r.width;
		int h = r.height;
		//	auto context = Context::GetInstance().m_drawContext;
		auto ctx = Context::GetInstance().m_drawContext->vg;
		auto theme = Context::GetInstance().m_drawContext->theme;
		NVGcolor colorTop = theme->buttonGradientTopUnfocused;
		NVGcolor colorBot = theme->buttonGradientBotUnfocused;
		NVGpaint bg = nvgLinearGradient(ctx, x, y, x, y + h, colorTop, colorBot);
		nvgBeginPath(ctx);
		nvgRoundedRect(ctx, x + 1, y + 1, w - 2, h - 2, cornerRadius - 1);
		nvgFillPaint(ctx, bg);
		nvgFill(ctx);
		
		g_segID = 0;
	}
	
	static void End()
	{
		constexpr float cornerRadius = 4.0f;
		int x = g_rect.x;
		int y = g_rect.y;
		int w = g_rect.width;
		int h = g_rect.height;
		auto context = Context::GetInstance().m_drawContext;
		auto ctx = Context::GetInstance().m_drawContext->vg;
		//	auto theme = Context::GetInstance().m_drawContext->theme;
		int width = w / g_segCount;
		nvgBeginPath(ctx);
		nvgRoundedRect(ctx, x + 0.5f, y + 0.5f, w - 1, h - 1, cornerRadius - 0.5f);
		nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 255));
		nvgStroke(ctx);
		
		for (int i = 1; i < g_segCount; ++i)
		{
			int xx = x + width*i;
			DrawLine(context, xx, y, xx, y+h, nvgRGBA(0, 0, 0, 255));
		}
	}
	
	static bool Button(const char* text, NVGcolor fontColor, bool active)
	{
		auto ctx = Context::GetInstance().m_drawContext->vg;
		auto input = Context::GetInstance().m_input;
		
		auto r = g_rect;
		r.width = g_rect.width / g_segCount;
		r.x += r.width*g_segID;
		int x = r.x;
		int y = r.y;
		int h = r.height;
		int w = r.width;

		bool inside = input->MouseInRect(r);
		//	bool pushed = inside && (input->GetMouseButton(MouseButton::Left));
		bool clicked = inside && input->GetMouseButtonUp(MouseButton::Left);
		bool pushed = clicked || (inside && (input->GetMouseButton(MouseButton::Left)));
		
		if (active || pushed)
		{
			auto theme = Context::GetInstance().m_drawContext->theme;
			auto colorTop = theme->buttonGradientTopFocused;
			auto colorBot = theme->buttonGradientBotFocused;
			if (pushed)
			{
				colorTop = theme->buttonGradientTopPushed;
				colorBot = theme->buttonGradientBotPushed;
			}
			
			// because of this scissor, do NOT insert any other draw functions between Begin() and Button()
			nvgScissor(ctx, x, y, w, h);
			auto bg = nvgLinearGradient(ctx, x, y, x, y + h, colorTop, colorBot);
			//	nvgBeginPath(ctx);
			nvgFillPaint(ctx, bg);
			nvgFill(ctx);
			nvgResetScissor(ctx);
		}
		
		//	nvgFontSize(ctx, fontSize);
		//	nvgFontFace(ctx, fontFace);
		nvgFillColor(ctx, fontColor);
		//	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		
		y += h/2;
		x += w/2;
		nvgText(ctx, x, y, text, nullptr);
		
//		g_rect.x += width;
		g_segID ++;
		return clicked;
	}
	
private:
	static FishGUI::Rect g_rect;
	static int g_segCount;
	static int g_segID;
	
	SegmentedButtons() = delete;
};

FishGUI::Rect SegmentedButtons::g_rect;
int SegmentedButtons::g_segCount;
int SegmentedButtons::g_segID;



void UnityToolBar::Draw()
{
	auto theme = Context::GetInstance().m_drawContext->theme;
	auto ctx = Context::GetInstance().m_drawContext->vg;
	char icon[8];
	// left
	constexpr int x_margin = 10;
	constexpr int y_margin = 4;
//	constexpr int x_padding = 2;
	constexpr int leftButtonWidth = 32;
	auto rect = m_rect;
	auto r = rect;
	r.x += x_margin;
	r.y += y_margin;
	r.height -= y_margin*2;
	
	auto white = nvgRGB(255, 255, 255);
	
	nvgFontFace(ctx, "ui");
	nvgFontSize(ctx, theme->standardFontSize);
//	nvgFillColor(ctx, fontColor);
	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	
	int icons[] = {ICO_HAND, ICO_MOVE, ICO_ROTATE, ICO_SCALE};
	r.width = leftButtonWidth * 4;
//	SegmentedButtons(4, nullptr, icons, r);
	static int activeID = 0;
	SegmentedButtons::Begin(r, 4);
	for (int i = 0; i < 4; ++i)
	{
		auto color = (activeID == i ? white : theme->textColor);
		if (SegmentedButtons::Button(CodePointToUTF8(icons[i], icon), color,  activeID == i))
		{
			activeID = i;
		}
	}
	SegmentedButtons::End();
//	{
//		bool btnClicked[4];
//		char icon[4][8];
//		SegmentedButton buttons[] = {
//			{CodePointToUTF8(icons[0], icon[0]), (activeID == 0 ? white : theme->textColor), activeID==0, btnClicked[0]},
//			{CodePointToUTF8(icons[1], icon[1]), (activeID == 1 ? white : theme->textColor), activeID==1, btnClicked[1]},
//			{CodePointToUTF8(icons[2], icon[2]), (activeID == 2 ? white : theme->textColor), activeID==2, btnClicked[2]},
//			{CodePointToUTF8(icons[3], icon[3]), (activeID == 3 ? white : theme->textColor), activeID==3, btnClicked[3]},
//		};
//		SegmentedButtons(4, buttons, r);
//		for (int i = 0; i < 4; ++i)
//		{
//			if (btnClicked[i])
//				activeID = i;
//		}
//	}
	
	const int anchor = r.x+r.width;
	
	
	// middle
	{
		const int mid = (rect.x + rect.width)/2;
		constexpr int buttonWidth = 32;
		r.width = buttonWidth * 3;
		r.x = mid - buttonWidth*3/2;
		
		NVGcolor color1 = theme->textColor;
		NVGcolor color2 = nvgRGB(23, 148, 250);
		static bool running = false;
		static bool paused = false;
		int icon1 = running ? ICO_STOP : ICO_START;
		auto color = running ? color2 : color1;
		
		SegmentedButtons::Begin(r, 3);
		
		if (SegmentedButtons::Button(CodePointToUTF8(icon1, icon), color,  running))
		{
			running = !running;
			if (!running)
				paused = false;
		}
		if (SegmentedButtons::Button(CodePointToUTF8(ICO_PAUSE, icon), color, paused))
		{
			paused = !paused;
		}
		if (SegmentedButtons::Button(CodePointToUTF8(ICO_NEXT, icon), color, false))
		{
			if (running)
				paused = true;
		}
		SegmentedButtons::End();
	}
	
	//
	{
		constexpr int buttonWidth = 64;
		r.x = anchor + 20;
		r.y += 2;
		r.height -= 2*2;
		r.width = buttonWidth * 2;
		
		static bool active1 = false;
		static bool active2 = false;
		nvgFontFace(ctx, "sans");
		nvgFontSize(ctx, theme->standardFontSize);
//		nvgFillColor(ctx, theme->textColor);
		
//		bool btn1Clicked = false;
//		bool btn2Clicked = false;
//		SegmentedButton buttons[] = {
//			{(active1 ? "Pivot":"Center"), theme->textColor, false, btn1Clicked},
//			{(active2 ? "Global":"local"), theme->textColor, false, btn2Clicked}
//		};
//
//		SegmentedButtons(2, buttons, r);
//
//		if (btn1Clicked)
//			active1 = !active1;
//		if (btn2Clicked)
//			active2 = !active2;
		
		SegmentedButtons::Begin(r, 2);
		if (SegmentedButtons::Button(active1 ? "Pivot" : "Center", theme->textColor, false))
		{
			active1 = !active1;
		}
		if (SegmentedButtons::Button(active2 ? "Global" : "Local", theme->textColor, false))
		{
			active2 = !active2;
		}
		SegmentedButtons::End();
	}
	
	// right
	{
		constexpr int buttonWidth = 78;
		r.x = rect.x + rect.width - x_margin - buttonWidth;
		r.width = buttonWidth;
		Button("Layout", r);
		
		r.x -= 12 + buttonWidth;
		Button("Layers", r);
	}
}
