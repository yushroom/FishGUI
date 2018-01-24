#pragma once

#include "ItemView.hpp"
#include "../FishGUI.hpp"
#include "../Input.hpp"
#include "../Draw.hpp"
#include "../Theme.hpp"
#include "../Utils.hpp"

#include <set>

namespace FishGUI
{
	template<class T>
	class TreeWidget : public TItemView<T>
	{
	protected:
		typedef TItemView<T> Super;

		T m_root = nullptr;
		std::set<T> m_unfolded;
		
		using Super::m_model;
		using Super::m_selectionModel;
		using Super::AppendVisibleItem;
		using IMWidget::m_imContext;
		using Widget::m_rect;
		
	public:

		TreeWidget(const char* name) : Super(name) {
		}

		virtual void Render() override
		{
			assert(m_model != nullptr);
			
			if (m_root == nullptr)
			{
				// null can also have children
//				Indent(10);
				if (m_model->ChildCount(nullptr) > 0)
				{
					for (int i = 0; i < m_model->ChildCount(nullptr); ++i)
					{
						Cell(m_model->ChildAt(i, nullptr));
					}
				}
//				Unindent(10);
			}
			else
			{
				Cell(m_root);
			}
			
			// handle aditional keyEvent
			auto e = this->m_keyEvent;
			auto item = m_selectionModel.CurrentSelected();
			if (e != nullptr && !e->isAccepted() && item != nullptr && e->type()==KeyEvent::Type::KeyPress)
			{
				int key = e->key();
				if (key == GLFW_KEY_RIGHT && m_model->ChildCount(item) > 0)
				{
					auto child = m_model->ChildAt(0, item);
					m_selectionModel.SelectItem(child, SelectionFlag::ClearAndSelect);
					m_unfolded.insert(item);
					e->Accept();
				}
				else if (key == GLFW_KEY_LEFT)
				{
					auto it = this->m_unfolded.find(item);
					if (it != this->m_unfolded.end())	// unfolded
					{
						// fold it
						this->m_unfolded.erase(it);
						e->Accept();
					}
					else
					{
						auto parent = m_model->Parent(item);
						if (parent != nullptr)
						{
							m_selectionModel.SelectItem(parent, SelectionFlag::ClearAndSelect);
							e->Accept();
						}
					}
//					m_selectionModel.selectItem(parent, SelectionFlag::ClearAndSelect);
//					m_unfolded.insert(item);
//					e->Accept();
				}
			}
		}

		void SetRoot(T root)
		{
			m_root = root;
		}

	protected:

		inline bool IsUnfolded(T item) const
		{
			return m_unfolded.find(item) != m_unfolded.end();
		}

		void Cell(T go)
		{
			auto it2 = m_unfolded.find(go);
			//bool isUnfolded = IsUnfolded(go);
			bool isUnfolded = (it2 != m_unfolded.end());

			bool outOfRange;
			auto rect = m_imContext->NextCell(IMGUIContext::cellHeight, outOfRange);
			AppendVisibleItem(go, rect);

			if (!outOfRange)
			{
				Rect preiconRect = rect;
				preiconRect.width = 20;

				auto ctx = Context::GetInstance().m_drawContext->vg;
				auto theme = Context::GetInstance().m_drawContext->theme;

				auto totalCellRect = rect;
				totalCellRect.x = m_rect.x;
				totalCellRect.width = m_rect.width;

				bool clickedPreicon = false;
				auto e = this->m_mouseEvent;
				if (e != nullptr && !e->isAccepted())
				{
					bool insidePreicon = PointInRect(e->pos(), preiconRect);
					clickedPreicon = insidePreicon && e->type() == MouseEvent::Type::MouseButtonPress;
				}

				bool selected = m_selectionModel.IsSelected(go);
				if (selected)
				{
					auto& color = m_imContext->widget->IsFocused() ? theme->selectionHighlightColor : theme->selectionColor;
					DrawRect(Context::GetInstance().m_drawContext, totalCellRect, color);
				}

				// preicon
				constexpr int preiconWidth = 20;
				constexpr int iconWidth = 20;

				int x = rect.x + preiconWidth / 2 + 2;
				int y = rect.y + 16 / 2;
				if (m_model->ChildCount(go) > 0)
				{
					if (clickedPreicon)
					{
						if (isUnfolded)
							m_unfolded.erase(it2);
						else
							m_unfolded.insert(go);
						e->Accept();
					}

		//			auto icon = m_model.icon(go);
					nvgFontSize(ctx, 32);
					nvgFontFace(ctx, "icons");
					nvgFillColor(ctx, theme->textColor);
					nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
					char icon[8];
					if (isUnfolded)
						nvgText(ctx, (float)x, (float)y, CodePointToUTF8(0x25be, icon), nullptr);
					else
						nvgText(ctx, (float)x, (float)y, CodePointToUTF8(0x25b8, icon), nullptr);
				//			nvgText(ctx, x, y, icon->fontText.c_str(), nullptr);
				}
				rect.x += preiconWidth;

				auto icon = m_model->Icon(go);
				if (icon != nullptr)
				{
					x += 3;
					icon->Draw(Context::GetInstance().m_drawContext, Rect{x, rect.y, 20, rect.height});
					rect.x += 14;
				}

				// text
//				rect.x += 2;
				rect.width -= 20 + 2;
				Label(m_model->Text(go), rect);
			}

			Indent(10);
			if (m_model->ChildCount(go) > 0 && isUnfolded)
			{
				for (int i = 0; i < m_model->ChildCount(go); ++i)
				{
					Cell(m_model->ChildAt(i, go));
				}
			}
			Unindent(10);
		}
	};
}
