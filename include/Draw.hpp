#pragma once

#include "nanovg.h"
#include "Vector.hpp"

//struct NVGcontext;
namespace FishGUI
{
	struct Theme;

	struct DrawContext
	{
		Theme* 			theme = nullptr;
		NVGcontext* 	vg = nullptr;
	};


	void DrawTabButton(DrawContext* ctx, const char* text, int x, int y, int w, int h, bool active);
	static void DrawTabButton(DrawContext* ctx, const char* text, const FishGUI::Rect& r, bool active)
	{
		DrawTabButton(ctx, text, r.x, r.y, r.width, r.height, active);
	}

	void DrawSearchBox(DrawContext* ctx, const char* text, float x, float y, float w, float h);

	void DrawDropDown(DrawContext* ctx, const char* text, float x, float y, float w, float h);
	static void DrawDropDown(DrawContext* ctx, const char* text, const FishGUI::Rect& r)
	{
		DrawDropDown(ctx, text, r.x, r.y, r.width, r.height);
	}

	void DrawLabel(DrawContext* ctx, const char* text, float x, float y, float w, float h, int alignment = NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

	static void DrawLabel(DrawContext* ctx, const char* text, const FishGUI::Rect& r, int alignment = NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE)
	{
		DrawLabel(ctx, text, r.x, r.y, r.width, r.height, alignment);
	}

	void DrawEditBox(DrawContext* ctx, const char* text, float x, float y, float w, float h);

	static void DrawEditBox(DrawContext* ctx, const char* text, const FishGUI::Rect& r)
	{
		DrawEditBox(ctx, text, r.x, r.y, r.width, r.height);
	}

	void DrawCheckBox(DrawContext* ctx, float x, float y, float w, float h);
	static void DrawCheckBox(DrawContext* ctx, const FishGUI::Rect& r)
	{
		DrawCheckBox(ctx, r.x, r.y, r.width, r.height);
	}

	void DrawButton(DrawContext* ctx, int preicon, const char* text, float x, float y, float w, float h);
	static void DrawButton(DrawContext* ctx, int preicon, const char* text, const FishGUI::Rect& r)
	{
		DrawButton(ctx, preicon, text, r.x, r.y, r.width, r.height);
	}

	void DrawSlider(DrawContext* ctx, float pos, float x, float y, float w, float h);
	static void DrawSlider(DrawContext* ctx, float pos, const FishGUI::Rect& r)
	{
		DrawSlider(ctx, pos, r.x, r.y, r.width, r.height);
	}


	void DrawSegmentedButtons(DrawContext* ctx, int count, const char* labels[], int icons[], float x, float y, float w, float h);
	static void DrawSegmentedButtons(DrawContext* ctx, int count, const char* labels[], int icons[], const FishGUI::Rect& r)
	{
		DrawSegmentedButtons(ctx, count, labels, icons, r.x, r.y, r.width, r.height);
	}


	void DrawLine(DrawContext* ctx, float x1, float y1, float x2, float y2);
	void DrawRect(DrawContext* ctx, float x, float y, float w, float h, const NVGcolor & color);
	void DrawRoundedRect(DrawContext* ctx, float x, float y, float w, float h, float r, const NVGcolor & color);
}
