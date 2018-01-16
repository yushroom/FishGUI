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
	m_selectionModel.SetItemsPerRow(1);
}

void FileListWidget::DrawImpl()
{
	m_selectionModel.BeforeFrame();
	
	constexpr int imageSize = 64;
	constexpr int textHight = 14;
	constexpr int pad = 6;
	int count = m_model.rowCount(m_root);
	
	if (count == 0)
		return;
	
	auto ctx = Context::GetInstance().m_drawContext;
	auto theme = ctx->theme;
	
	// rect of each cell (including padding)
	auto r = m_rect;
	r.x = m_rect.x;
	r.y = m_rect.y + m_imContext->yStart;
	r.width = imageSize + pad*2;
	r.height = pad + imageSize + textHight + pad;

	float totalWidth = m_imContext->Right() - m_rect.x;
	int itemsPerRow = std::floor( totalWidth / r.width );
	m_selectionModel.SetItemsPerRow(itemsPerRow);

	bool outOfRange = false;
	m_imContext->NextCell(r.height, outOfRange);
	
	for (int i = 0; i < count; ++i)
	{
		if (r.right() > m_imContext->Right())
		{
			r.x = m_rect.x;
			r.y += r.height;
			m_imContext->NextCell(r.height, outOfRange);
		}

		auto node = m_model.childAt(m_root, i);
		m_selectionModel.AppendVisibleItem(node);
		if (!outOfRange)
		{
			if (m_mouseEvent != nullptr && !m_mouseEvent->isAccepted())
			{
				auto e = m_mouseEvent;
				bool inside = PointInRect(e->pos(), r);
				bool clicked = inside && e->type()==MouseEvent::Type::MouseButtonPress;
			
				if (clicked)
					m_selectionModel.OnItemClicked(node, e);
			}

			if (m_selectionModel.IsSelected(node))
			{
				auto& color = m_isFocused ? theme->selectionHighlightColor : theme->selectionColor;
				DrawRect(ctx, r, color);
			}

		
			auto imageRect = r;
			imageRect.x += pad;
			imageRect.y += pad;
			imageRect.width = imageSize;
			imageRect.height = imageSize;
			FishGUI::DrawRect(ctx, imageRect, nvgRGB(255, 255, 255));
			//FishGUI::DrawImage(ctx, 2, imageRect);
			
			const auto& text = m_model.childAt(m_root, i)->fileName;
			auto textRect = r;
			textRect.x += pad;
			textRect.y += pad + imageSize;
			textRect.width = imageSize;
			textRect.height = textHight;
			FishGUI::Label(text, textRect);
		}
		
		r.x += r.width;
	}
	
	m_selectionModel.AfterFrame(m_mouseEvent, m_keyEvent);
}
