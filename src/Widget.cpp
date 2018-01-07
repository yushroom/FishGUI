#include "Widget.hpp"

#include <iostream>
#include <algorithm>

#include "Input.hpp"
#include "FishGUI.hpp"

namespace FishGUI
{
	//void CalcSplitRect(
	//	Orientation orientation, 
	//	Alignment alignment1, 
	//	Alignment alignment2, 
	//	const Rect & rect, 
	//	int middleInterval, 
	//	Rect& out1, Rect& out2)
	//{
	//	if (orientation == Orientation::Horizontal)	// left and right
	//	{
	//		if (alignment1 == Alignment::Maximum)
	//		{
	//			
	//		}
	//	}
	//}

	void Widget::Draw()
	{
		if (m_layout == nullptr)
			return;
		m_layout->PerformLayout(m_rect);
		//for (auto child : m_children)
		//	child->Draw();
	}

	void Widget::SetLayout(Layout* layout)
	{
		m_layout = layout;
		//m_layout->m_widget = this;
	}
	
	void SimpleLayout::PerformLayout(const Rect& rect)
	{
		if (m_widget == nullptr)
			return;
		m_widget->m_rect = rect;
		m_widget->Draw();
	}

	bool Splitter::MouseDragEvent()
	{
		auto& input = Input::GetInstance();
		if (m_orientation == Orientation::Horizontal)
			m_pos = input.m_mousePosition.x;
		else
			m_pos = input.m_mousePosition.y;
		m_dirty = true;
		return true;
	}

	void SplitLayout::PerformLayout(const Rect& rect)
	{
		assert(m_done);

		auto ms1 = part1.GetMinSize();
		auto ms2 = part2.GetMinSize();
		if (m_orientation == Orientation::Horizontal)
		{
			m_minSize.width = ms1.width + ms2.width + INTERVAL;
			m_minSize.height = std::max(ms1.height, ms2.height);
		}
		else
		{
			m_minSize.width = std::max(ms1.width, ms2.width);
			m_minSize.height = ms1.height + ms2.height + INTERVAL;
		}

		int size;
		if (m_orientation == Orientation::Horizontal)
			size = rect.width - INTERVAL;
		else
			size = rect.height - INTERVAL;
		int size1 = size * 0.5f;
		auto r1 = rect;
		auto r2 = rect;

		if (part1.type == PartType::EWidget)
		{
			if (part2.type == PartType::EWidget)
			{
				int s1, s2;
				if (m_orientation == Orientation::Horizontal)
				{
					s1 = part1.w->GetWidth();
					s2 = part2.w->GetWidth();
				}
				else
				{
					s1 = part1.w->GetHeight();
					s2 = part2.w->GetHeight();
				}
				size1 = size * float(s1) / (s1 + s2);
			}
			else
			{
				size1 = part1.w->GetWidth();
			}
		}
		else
		{
			if (part2.type == PartType::EWidget)
			{
				if (m_orientation == Orientation::Horizontal)
				{
					size1 = size - part2.w->GetWidth();
				}
				else
				{
					size1 = size - part2.w->GetHeight();
				}
			}
			else
			{
				size1 = size * 0.5f;
			}
		}

		if (m_orientation == Orientation::Horizontal)
		{
			size1 = std::max(size1, ms1.width);
			size1 = std::min(size1, size - ms2.width);
		}
		else
		{
			size1 = std::max(size1, ms1.height);
			size1 = std::min(size1, size - ms2.height);
		}

		if (m_splitter.m_dirty)
		{
			if (m_orientation == Orientation::Horizontal)
			{
				size1 = m_splitter.m_pos - rect.x;
			}
			else
			{
				size1 = m_splitter.m_pos - rect.y;
			}
		}

		if (m_orientation == Orientation::Horizontal)
		{
			r1.width = size1;
			r2.x = rect.x + size1 + INTERVAL;
			r2.width = size - size1;
		}
		else
		{
			r1.height = size1;
			r2.y = rect.y + size1 + INTERVAL;
			r2.height = size - size1;
		}

		if (part1.type == PartType::EWidget)
		{
			part1.w->m_rect = r1;
			part1.w->Draw();
		}
		else
		{
			part1.l->PerformLayout(r1);
		}

		if (part2.type == PartType::EWidget)
		{
			part2.w->m_rect = r2;
			part2.w->Draw();
		}
		else
		{
			part2.l->PerformLayout(r2);
		}

		auto& input = Input::GetInstance();
		//Rect& splitterRect = m_splitter.m_rect;
		//splitterRect = rect;
		Rect splitterRect = rect;
		constexpr int padding = 2;
		if (m_orientation == Orientation::Horizontal)
		{
			splitterRect.x += size1 - padding;
			splitterRect.width = INTERVAL + padding*2;
		}
		else
		{
			splitterRect.y += size1 - padding;
			splitterRect.height = INTERVAL + padding * 2;
		}
		if (input.MouseInRect(splitterRect))
		{
			auto type = m_orientation == Orientation::Horizontal ? CursorType::HResize : CursorType::VResize;
			Cursor::GetInstance().SetCursorType(type);
			if (input.GetMouseButtonDown(MouseButton::Left))
			{
				input.m_dragWidget = &m_splitter;
			}
		}
		if (m_orientation == Orientation::Horizontal)
			m_splitter.m_pos = rect.x + size1;
		else
			m_splitter.m_pos = rect.y + size1;
		m_splitter.m_dirty = false;
	}
}
