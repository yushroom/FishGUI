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

#include "Draw.hpp"
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "Theme.hpp"

#define ICON_SEARCH 0x1F50D
#define ICON_CIRCLED_CROSS 0x2716
#define ICON_CHEVRON_RIGHT 0xE75E
#define ICON_CHECK 0x2713
#define ICON_LOGIN 0xE740
#define ICON_TRASH 0xE729

extern Theme theme;

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
////	nvgClearState(ctx);
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


void DrawSearchBox(NVGcontext* ctx, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = h / 2 - 1;
	
	// Edit
	bg = nvgBoxGradient(ctx, x, y + 1.5f, w, h, h / 2, 5, nvgRGBA(0, 0, 0, 16), nvgRGBA(0, 0, 0, 92));
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x, y, w, h, cornerRadius);
	nvgFillPaint(ctx, bg);
	nvgFill(ctx);
	
	/*	nvgBeginPath(ctx);
	 nvgRoundedRect(ctx, x+0.5f,y+0.5f, w-1,h-1, cornerRadius-0.5f);
	 nvgStrokeColor(ctx, nvgRGBA(0,0,0,48));
	 nvgStroke(ctx);*/
	
	nvgFontSize(ctx, h*1.3f);
	nvgFontFace(ctx, "icons");
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 64));
	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(ctx, x + h * 0.55f, y + h * 0.55f, cpToUTF8(ICON_SEARCH, icon), NULL);
	
	nvgFontSize(ctx, 20.0f);
	nvgFontFace(ctx, "sans");
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 32));
	
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgText(ctx, x + h * 1.05f, y + h * 0.5f, text, NULL);
	
	nvgFontSize(ctx, h*1.3f);
	nvgFontFace(ctx, "icons");
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 32));
	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(ctx, x + w - h * 0.55f, y + h * 0.55f, cpToUTF8(ICON_CIRCLED_CROSS, icon), NULL);
}

void DrawDropDown(NVGcontext* ctx, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = 4.0f;
	
	bg = nvgLinearGradient(ctx, x, y, x, y + h, nvgRGBA(255, 255, 255, 16), nvgRGBA(0, 0, 0, 16));
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x + 1, y + 1, w - 2, h - 2, cornerRadius - 1);
	nvgFillPaint(ctx, bg);
	nvgFill(ctx);
	
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x + 0.5f, y + 0.5f, w - 1, h - 1, cornerRadius - 0.5f);
	nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 48));
	nvgStroke(ctx);
	
	nvgFontSize(ctx, 20.0f);
	nvgFontFace(ctx, "sans");
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 160));
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgText(ctx, x + h * 0.3f, y + h * 0.5f, text, NULL);
	
	nvgFontSize(ctx, h*1.3f);
	nvgFontFace(ctx, "icons");
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 64));
	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(ctx, x + w - h * 0.5f, y + h * 0.5f, cpToUTF8(ICON_CHEVRON_RIGHT, icon), NULL);
}

void DrawLabel(NVGcontext* ctx, const char* text, float x, float y, float w, float h, int alignment /*= NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE*/)
{
	NVG_NOTUSED(w);
	
	nvgFontSize(ctx, theme.mStandardFontSize);
	nvgFontFace(ctx, "sans");
	nvgFillColor(ctx, theme.mTextColor);
	
	// debug
//	nvgBeginPath(ctx);
//	nvgRect(ctx, x, y, w, h);
//	nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 255));
//	nvgStroke(ctx);
	
	nvgTextAlign(ctx, alignment);
	nvgSave(ctx);
	nvgIntersectScissor(ctx, x, y, w, h);
	nvgText(ctx, x, y + h * 0.5f, text, nullptr);
	nvgRestore(ctx);
}

void DrawEditBoxBase(NVGcontext* ctx, float x, float y, float w, float h)
{
	NVGpaint bg;
	// Edit
	bg = nvgBoxGradient(ctx, x + 1, y + 1 + 1.5f, w - 2, h - 2, 3, 4, nvgRGBA(255, 255, 255, 32), nvgRGBA(32, 32, 32, 32));
	nvgBeginPath(ctx);
//	nvgRoundedRect(ctx, x + 1, y + 1, w - 2, h - 2, 4 - 1);
	nvgRect(ctx, x + 1, y + 1, w - 2, h - 2);
	nvgFillPaint(ctx, bg);
	nvgFill(ctx);
	
	nvgBeginPath(ctx);
//	nvgRoundedRect(ctx, x + 0.5f, y + 0.5f, w - 1, h - 1, 4 - 0.5f);
	nvgRect(ctx, x + 0.5f, y + 0.5f, w - 1, h - 1);
	nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 48));
	nvgStroke(ctx);
}

void DrawEditBox(NVGcontext* ctx, const char* text, float x, float y, float w, float h)
{
	DrawEditBoxBase(ctx, x, y, w, h);
	
	nvgSave(ctx);
	nvgIntersectScissor(ctx, x, y, w, h);
	
	nvgFontSize(ctx, theme.mStandardFontSize);
	nvgFontFace(ctx, "sans");
//	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 64));
	nvgFillColor(ctx, theme.mTextColor);
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgText(ctx, x + h * 0.3f, y + h * 0.5f, text, NULL);
	
	nvgRestore(ctx);
}

void DrawCheckBox(NVGcontext* ctx, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	NVG_NOTUSED(w);
	
//	nvgFontSize(ctx, 18.0f);
//	nvgFontFace(ctx, "sans");
//	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 160));
//	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
//	nvgText(ctx, x + 28, y + h * 0.5f, text, NULL);
	
	constexpr int size = 14;
	bg = nvgBoxGradient(ctx, x + 1, y + (int)(h*0.5f) - size/2 + 1, size, size, 3, 3, nvgRGBA(251, 251, 251, 255), nvgRGBA(188, 188, 188, 255));
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x + 1, y + (int)(h*0.5f) - size/2, size, size, 3);
	nvgFillPaint(ctx, bg);
	nvgFill(ctx);
	
	nvgFontSize(ctx, 30);
	nvgFontFace(ctx, "icons");
	nvgFillColor(ctx, theme.mTextColor);
	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(ctx, x + size/2 + 2, y + h * 0.5f, "✓", NULL);
}

void DrawButton(NVGcontext* ctx, int preicon, const char* text, float x, float y, float w, float h)
{
	NVGpaint bg;
	char icon[8];
	float cornerRadius = 4.0f;
	float tw = 0, iw = 0;
	
	//	bg = nvgLinearGradient(ctx, x,y,x,y+h, nvgRGBA(255,255,255,isBlack(col)?16:32), nvgRGBA(0,0,0,isBlack(col)?16:32));
	NVGcolor colorTop = theme.mButtonGradientTopUnfocused;
	NVGcolor colorBot = theme.mButtonGradientBotUnfocused;
	bg = nvgLinearGradient(ctx, x, y, x, y + h, colorTop, colorBot);
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x + 1, y + 1, w - 2, h - 2, cornerRadius - 1);
	//	if (!isBlack(col)) {
	//		nvgFillColor(ctx, col);
	//		nvgFill(ctx);
	//	}
	nvgFillPaint(ctx, bg);
	nvgFill(ctx);
	
//	nvgBeginPath(ctx);
//	nvgRoundedRect(ctx, x + 0.5f, y + 0.5f, w - 1, h - 1, cornerRadius - 0.5f);
//	nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 48));
//	nvgStroke(ctx);
	
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
//	nvgFillColor(ctx, nvgRGBA(0, 0, 0, 160));
//	nvgText(ctx, x + w * 0.5f - tw * 0.5f + iw * 0.25f, y + h * 0.5f - 1, text, NULL);
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 160));
	nvgText(ctx, x + w * 0.5f - tw * 0.5f + iw * 0.25f, y + h * 0.5f, text, NULL);
	
	nvgRestore(ctx);
}

void DrawSlider(NVGcontext* ctx, float pos, float x, float y, float w, float h)
{
	NVGpaint bg, knob;
	float cy = y + (int)(h*0.5f);
	float kr = (int)(h*0.25f);
	
	nvgSave(ctx);
	//	nvgClearState(ctx);
	
	// Slot
	bg = nvgBoxGradient(ctx, x, cy - 2 + 1, w, 4, 2, 2, nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 128));
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x, cy - 2, w, 4, 2);
	nvgFillPaint(ctx, bg);
	nvgFill(ctx);
	
	// Knob Shadow
//	bg = nvgRadialGradient(ctx, x + (int)(pos*w), cy + 1, kr - 3, kr + 3, nvgRGBA(0, 0, 0, 64), nvgRGBA(0, 0, 0, 0));
//	nvgBeginPath(ctx);
//	nvgRect(ctx, x + (int)(pos*w) - kr - 5, cy - kr - 5, kr * 2 + 5 + 5, kr * 2 + 5 + 5 + 3);
//	nvgCircle(ctx, x + (int)(pos*w), cy, kr);
//	nvgPathWinding(ctx, NVG_HOLE);
//	nvgFillPaint(ctx, bg);
//	nvgFill(ctx);
	
	// Knob
	knob = nvgLinearGradient(ctx, x, cy - kr, x, cy + kr, nvgRGBA(255, 255, 255, 16), nvgRGBA(0, 0, 0, 16));
	nvgBeginPath(ctx);
	nvgCircle(ctx, x + (int)(pos*w), cy, kr - 1);
	nvgFillColor(ctx, nvgRGBA(40, 43, 48, 255));
	nvgFill(ctx);
	nvgFillPaint(ctx, knob);
	nvgFill(ctx);
	
	nvgBeginPath(ctx);
	nvgCircle(ctx, x + (int)(pos*w), cy, kr - 0.5f);
	nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 92));
	nvgStroke(ctx);
	
	nvgRestore(ctx);
}

void DrawLine(NVGcontext* ctx, float x1, float y1, float x2, float y2)
{
	nvgBeginPath(ctx);
	nvgMoveTo(ctx, x1, y1);
	nvgLineTo(ctx, x2, y2);
	nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 255));
	nvgStroke(ctx);
}

void DrawRect(NVGcontext* ctx, float x, float y, float w, float h, const NVGcolor & color)
{
	nvgBeginPath(ctx);
	nvgRect(ctx, x, y, w, h);
	nvgFillColor(ctx, color);
	nvgFill(ctx);
}

void DrawRoundedRect(NVGcontext* ctx, float x, float y, float w, float h, float r, const NVGcolor & color)
{
	nvgBeginPath(ctx);
	nvgRoundedRect(ctx, x, y, w, h, r);
	nvgFillColor(ctx, color);
	nvgFill(ctx);
}
