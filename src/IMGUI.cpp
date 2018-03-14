#include <cassert>
#include <sstream>

#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Draw.hpp>
#include <FishGUI/Theme.hpp>
#include <FishGUI/Math.hpp>
#include <FishGUI/Widget.hpp>
#include <FishGUI/Input.hpp>

namespace FishGUI
{
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
	//  if size1 > 0 and size2 > 0: width of left = (r.width - interval) * size1 / (size1+size2))
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
		else    // size2 < 0
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
//          Label("yStart="+ToString(yStart)+" pos.y="+std::to_string(pos.y) + " rect.bottom="+std::to_string(rect.y+rect.height));
//          Label("yStart="+ToString(yStart)+" pos.y="+std::to_string(pos.y) + " totalHeight="+std::to_string(totalHeight));
		auto ctx = GetDrawContext();
		auto theme = CurrentTheme();
		if (showScrollBar)
		{
			int x = rect.x+rect.width-scrollBarWidth;
			int y = rect.y;
			int w = scrollBarWidth;
			int h = rect.height;
			DrawRect(ctx, (float)x, (float)y, (float)w, (float)h, theme->scrollBarBackColor);
			
//              const int total_height = pos.y - (yStart+rect.y);
			float p = float(rect.height) / totalHeight;
			constexpr int pad = 1;
			x = rect.x+rect.width-scrollBarWidth+pad;
//              y = rect.y + float(rect.height)*(-yStart)/total_height;
			y = static_cast<int>( rect.y + (-yStart)*p );
			w = scrollBarWidth - pad*2;
			h = static_cast<int>(p * rect.height);
			int r = scrollBarWidth / 2 - pad;
			DrawRoundedRect(ctx, (float)x, (float)y, (float)w, (float)h, (float)r, theme->scrollBarColor);
			
			auto input = Input::GetCurrent();
			if (input->m_scrolling && input->MouseInRect(rect))
			{
				yStart += input->m_scroll.y*4;
//                  printf("%lf\n", yStart);
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
//          totalHeight += ymargin;
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
	void IMGUIGuard()
	{
		if (g_IMContext == nullptr)
		{
			printf("This function can be call inside a IMGUI call\n");
			abort();
		}
	}
	
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
//              color1 = CurrentTheme()->buttonGradientTopFocused;
//              color2 = CurrentTheme()->buttonGradientBotFocused;
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
//              color1 = CurrentTheme()->buttonGradientTopPushed;
//              color2 = CurrentTheme()->buttonGradientBotPushed;
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

	void Text(const char* label, const std::string& text)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		bool outOfRange;
		g_IMContext->NextCell2(r1, r2, outOfRange);
		if (outOfRange)
			return;
		DrawLabel(ctx, label, r1);
		DrawEditBox(ctx, text.c_str(), r2);
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

	bool Float(const std::string & label, float& value)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		bool outOfRange;
		g_IMContext->NextCell2(r1, r2, outOfRange);
		if (outOfRange)
			return false;

		DrawLabel(ctx, label.c_str(), r1);
		auto fstr = ToString(value);
		DrawEditBox(ctx, fstr.c_str(), r2);
		return false;
	}

	
	void Image(unsigned int image, int width, int height, bool flip)
	{
		assert(g_IMContext->widget != nullptr);
//      auto ctx = GetDrawContext();
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
	
	bool Float3(const std::string & label, float& x, float& y, float& z)
	{
		assert(g_IMContext->widget != nullptr);
		auto ctx = GetDrawContext();
		Rect r1, r2;
		bool outOfRange;
		g_IMContext->NextCell2(r1, r2, outOfRange, 1, 3);
		if (outOfRange)
			return false;
		
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
		
		return false;
	}
}
