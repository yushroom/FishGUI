#pragma once

#include <string>
#include <nanovg.h>
#include "Math.hpp"

#include "Draw.hpp"
#include "Theme.hpp"

namespace FishGUI
{
	struct DrawContext;

	class Icon
	{
	public:
		virtual void Draw(DrawContext* context, const Rect& rect) const = 0;

	protected:
	};

	class FontIcon : public Icon
	{
	public:

		FontIcon(const std::string& text, float fontSize, const char* m_fontFace)
			: m_text(text), m_fontSize(fontSize), m_fontFace(m_fontFace)
		{
		}

		virtual void Draw(DrawContext* context, const Rect& rect) const override
		{
			auto ctx = context->vg;
			nvgFontSize(ctx, m_fontSize);
			nvgFontFace(ctx, m_fontFace);
			nvgFillColor(ctx, context->theme->textColor);
			nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			nvgText(ctx, (float)rect.x+rect.width/2, (float)rect.y+rect.height/2, m_text.c_str(), nullptr);
		}

	protected:
		//	int codePoint;	// code point in .ttf
		std::string m_text;	// utf8
		float 		m_fontSize;
		const char* m_fontFace;
	};

	/*
	class ImageIcon : public Icon
	{
	public:
		virtual void Draw(const Rect& rect) override;
	protected:
		unsigned int m_texture = 0;	// opengl texture
	};
	*/
}