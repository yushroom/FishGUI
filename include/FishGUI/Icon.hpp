#pragma once

#include <string>
#include <nanovg/nanovg.h>
#include "Math.hpp"

#include "Draw.hpp"
#include "Theme.hpp"

#include "FishGUI.hpp"

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

	
	class ImageIcon : public Icon
	{
	public:
		static ImageIcon* FromFile(const std::string& path)
		{
			auto icon = new ImageIcon();
//			m_texture = nvgCreateImage(context->vg, path.c_str(), 0);
			icon->m_path = path;
			return icon;
		}
		
		virtual void Draw(DrawContext* context, const Rect& r) const override
		{
			auto ctx = context->vg;
			if (m_texture == -1)
				m_texture = nvgCreateImage(context->vg, m_path.c_str(), 0);
//			FishGUI::Image(5, rect);
			auto imgPaint = nvgImagePattern(ctx, r.x, r.y, r.width, r.height, 0, m_texture, 1);
			nvgBeginPath(ctx);
			nvgRect(ctx, r.x, r.y, r.width, r.height);
			nvgFillPaint(ctx, imgPaint);
			nvgFill(ctx);
		}
		
	protected:
		ImageIcon() = default;
		mutable int m_texture = -1;	// opengl texture
		std::string m_path;
	};
}
