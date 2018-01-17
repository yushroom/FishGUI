#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)

#include "nanovg.h"
#include "Math.hpp"
#include "FishGUI.hpp"

//struct NVGcontext;

namespace FishGUI
{
	struct Theme;

	struct DrawContext
	{
		Theme* 			theme = nullptr;
		NVGcontext* 	vg = nullptr;
		
		DrawContext() = default;
		DrawContext(DrawContext&) = delete;
		DrawContext& operator=(DrawContext&) = delete;
	};


	void DrawTabButton(DrawContext* ctx, const char* text, int x, int y, int w, int h, bool active);
	inline void DrawTabButton(DrawContext* ctx, const char* text, const FishGUI::Rect& r, bool active)
	{
		DrawTabButton(ctx, text, r.x, r.y, r.width, r.height, active);
	}

	void DrawSearchBox(DrawContext* ctx, const char* text, float x, float y, float w, float h);

	void DrawDropDown(DrawContext* ctx, const char* text, float x, float y, float w, float h);
	inline void DrawDropDown(DrawContext* ctx, const char* text, const FishGUI::Rect& r)
	{
		DrawDropDown(ctx, text, r.x, r.y, r.width, r.height);
	}

	void DrawLabel(DrawContext* ctx, const char* text, float x, float y, float w, float h, int alignment = NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

	inline void DrawLabel(DrawContext* ctx, const char* text, const FishGUI::Rect& r, int alignment = NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE)
	{
		DrawLabel(ctx, text, r.x, r.y, r.width, r.height, alignment);
	}

	void DrawEditBox(DrawContext* ctx, const char* text, float x, float y, float w, float h);

	inline void DrawEditBox(DrawContext* ctx, const char* text, const FishGUI::Rect& r)
	{
		DrawEditBox(ctx, text, r.x, r.y, r.width, r.height);
	}

	void DrawCheckBox(DrawContext* ctx, float x, float y, float w, float h, bool checked);
	inline void DrawCheckBox(DrawContext* ctx, const FishGUI::Rect& r, bool checked)
	{
		DrawCheckBox(ctx, r.x, r.y, r.width, r.height, checked);
	}

	void DrawButton(DrawContext* ctx, const char* text, float x, float y, float w, float h, NVGcolor colorTop, NVGcolor colorBot);
	void DrawButton(DrawContext* ctx, const char* text, float x, float y, float w, float h);
	inline void DrawButton(DrawContext* ctx, const char* text, const FishGUI::Rect& r)
	{
		DrawButton(ctx, text, r.x, r.y, r.width, r.height);
	}
	inline void DrawButton(DrawContext* ctx, const char* text, const Rect& r, NVGcolor colorTop, NVGcolor colorBot)
	{
		DrawButton(ctx, text, r.x, r.y, r.width, r.height, colorTop, colorBot);
	}

	void DrawSlider(DrawContext* ctx, float pos, float x, float y, float w, float h);
	inline void DrawSlider(DrawContext* ctx, float pos, const FishGUI::Rect& r)
	{
		DrawSlider(ctx, pos, r.x, r.y, r.width, r.height);
	}


//	void DrawSegmentedButtons(DrawContext* ctx, int count, SegmentedButton buttons[], float x, float y, float w, float h);
//	inline void DrawSegmentedButtons(DrawContext* ctx, int count, SegmentedButton buttons[], const FishGUI::Rect& r)
//	{
//		DrawSegmentedButtons(ctx, count, buttons, r.x, r.y, r.width, r.height);
//	}
	void DrawImage(DrawContext* context, unsigned int image, const Rect& r, bool flip = false);

	void DrawLine(DrawContext* ctx, float x1, float y1, float x2, float y2, const NVGcolor& color);
	void DrawRect(DrawContext* ctx, float x, float y, float w, float h, const NVGcolor & color);
	inline void DrawRect(DrawContext* ctx, const Rect& r, const NVGcolor & color)
	{
		DrawRect(ctx, r.x, r.y, r.width, r.height, color);
	}
	void DrawRoundedRect(DrawContext* ctx, float x, float y, float w, float h, float r, const NVGcolor & color);
}

	
#pragma warning(pop)