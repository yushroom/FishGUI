#include "FileListWidget.hpp"

#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Draw.hpp>
#include <FishGUI/Input.hpp>
#include <FishGUI/Theme.hpp>

using namespace FishGUI;

FileListWidget::FileListWidget(const char* name)
	: IMWidget(name)
{
	m_imContext->ymargin = 0;
//	m_imContext->xmargin = 0;
	m_selectionModel.SetSelectionType(SelectionType::Multi);
}

void FileListWidget::DrawImpl()
{
	MouseEvent e;
	auto input = Context::GetInstance().m_input;
	bool inside = input->MouseInRect(m_rect);
	bool clicked = inside && input->GetMouseButtonUp(MouseButton::Left);
	if (clicked)
	{
		e.button = static_cast<int>(MouseButton::Left);
		e.pos = input->GetMousePosition();
		e.state = MouseButtonState::Up;
		e.isAccepted = false;
		e.modifiers = input->m_mouseEventModifiers[0];
	}
	else
	{
		// no mouse event, just set isAccepted = true
		e.isAccepted = true;
	}
	
	m_selectionModel.BeforeFrame();
	
	constexpr int imageSize = 90;
	constexpr int textHight = 14;
	constexpr int pad = 6;
	int count = m_model.rowCount(m_root);
	
	if (count == 0)
		return;
	
	auto ctx = Context::GetInstance().m_drawContext;
	auto theme = ctx->theme;
	
	// rect of each cell (including padding)
	Rect r = m_rect;
	r.x = m_rect.x;
	r.y = m_rect.y + m_imContext->yStart;
	r.width = imageSize + pad*2;
	r.height = pad + imageSize + textHight + pad;

	bool outOfRange = false;
	m_imContext->NextCell(r.height, outOfRange);
	
//	auto input = Context::GetInstance().m_input;
	
	for (int i = 0; i < count; ++i)
	{
		auto node = m_model.childAt(m_root, i);
		m_selectionModel.AppendVisibleItem(node);
		if (!outOfRange)
		{
			bool inside = input->MouseInRect(r);
			bool clicked = inside && input->GetMouseButtonUp(MouseButton::Left);
		
			if (clicked)
				m_selectionModel.OnItemClicked(node, &e);
			if (m_selectionModel.IsSelected(node))
			{
				DrawRect(ctx, r, theme->selectionHighlightColor);
			}
		
			Rect imageRect = r;
			imageRect.x += pad;
			imageRect.y += pad;
			imageRect.width = imageSize;
			imageRect.height = imageSize;
			FishGUI::DrawRect(ctx, imageRect, nvgRGB(255, 0, 0));
			const auto& text = m_model.childAt(m_root, i)->fileName;

			Rect textRect = r;
			textRect.x += pad;
			textRect.y += pad + imageSize;
			textRect.width = imageSize;
			textRect.height = textHight;
			FishGUI::Label(text, textRect);
		}
		
		r.x += r.width;
		if (r.right() > m_imContext->Right())
		{
			r.x = m_rect.x;
			r.y += r.height;
			m_imContext->NextCell(r.height, outOfRange);
		}
	}
	
	m_selectionModel.AfterFrame(&e);
}
