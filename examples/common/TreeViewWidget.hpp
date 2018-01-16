#pragma once

#include <vector>
#include <set>
#include <FishGUI/Widget.hpp>

#include "TSelectionModel.hpp"
#include "ItemModel.hpp"

#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Input.hpp>
#include <FishGUI/Draw.hpp>
#include <FishGUI/Theme.hpp>
#include <FishGUI/Utils.hpp>

namespace FishGUI
{
	template<class ItemType>
	class TreeWidget : public IMWidget
	{
	public:
		typedef TTreeModel<ItemType, std::string> ModelType;
		typedef TSelectionModel<ItemType> SelectionModelType;
		
		TreeWidget(const char* name) : IMWidget(name)
		{
		}
		
		virtual void DrawImpl() override
		{
			MouseEvent e;
			auto input = Context::GetInstance().m_input;
//			auto theme = Context::GetInstance().m_drawContext->theme;
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
			
			Indent(10);
			Cell(m_root, &e);
			Unindent(10);
			
			m_selectionModel.AfterFrame(&e);
		}
		
		virtual void SetRoot(ItemType* root)
		{
			m_root = root;
		}
		
		SelectionModelType* GetSelectionModel()
		{
			return &m_selectionModel;
		}

	protected:
		void Cell(ItemType* go, MouseEvent* e)
		{
//			m_selectionModel.m_visiableItems.push_back(go);
			m_selectionModel.AppendVisibleItem(go);
			auto it2 = m_unfolded.find(go);
			bool isUnfolded = (it2 != m_unfolded.end());
			//	bool isUnfolded = !m_model.IsItemFolded(go);
			
			bool outOfRange;
			auto rect = m_imContext->NextCell(IMGUIContext::cellHeight, outOfRange);
			if (!outOfRange)
			{
				Rect preiconRect = rect;
				preiconRect.width = 20;
				
				auto ctx = Context::GetInstance().m_drawContext->vg;
				auto theme = Context::GetInstance().m_drawContext->theme;
				
				auto totalCellRect = rect;
				totalCellRect.x = m_rect.x;
				totalCellRect.width = m_rect.width;
				
				bool insidePreicon = PointInRect(e->pos, preiconRect);
				bool inside = (!insidePreicon) && PointInRect(e->pos, totalCellRect);
				bool clicked = inside && e->state == MouseButtonState::Up;
				bool clickedPreicon = false;
				
				if (clicked)
				{
					m_selectionModel.OnItemClicked(go, e);
				}
				
				bool selected = m_selectionModel.IsSelected(go);
				if (selected)
				{
					DrawRect(Context::GetInstance().m_drawContext, totalCellRect, theme->selectionHighlightColor);
				}
				
				// preicon
				int x = rect.x + 30/2 + 2;
				int y = rect.y + 16/2;
				if (m_model.hasChildren(go))
				{
					clickedPreicon = insidePreicon && e->state == MouseButtonState::Up;
					if (clickedPreicon)
					{
						if (isUnfolded)
							m_unfolded.erase(it2);
						//					m_model.FoldItem(go);
						else
							m_unfolded.insert(go);
						//					m_model.UnfoldItem(go);
						e->isAccepted = true;
					}
					
					//			auto icon = m_model.icon(go);
					nvgFontSize(ctx, 32);
					nvgFontFace(ctx, "icons");
					nvgFillColor(ctx, theme->textColor);
					nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
					char icon[8];
					if (isUnfolded)
						//nvgText(ctx, x, y, "▾", nullptr);
						nvgText(ctx, x, y, CodePointToUTF8(0x25be, icon), nullptr);
					else
						//nvgText(ctx, x, y, "▸", nullptr);
						nvgText(ctx, x, y, CodePointToUTF8(0x25b8, icon), nullptr);
					//			nvgText(ctx, x, y, icon->fontText.c_str(), nullptr);
				}
				
				// text
				rect.x += 20+2;
				rect.width -= 20 + 2;
				Label(m_model.data(go), rect);
			}
			
			Indent(10);
			if (m_model.hasChildren(go) && isUnfolded)
			{
				for (int i = 0; i < m_model.rowCount(go); ++i)
				{
					Cell(m_model.childAt(go, i), e);
				}
			}
			Unindent(10);
		}


		ItemType* m_root = nullptr;

		ModelType 			m_model;
		SelectionModelType	m_selectionModel;
		std::set<ItemType*>	m_unfolded;
	};
}
