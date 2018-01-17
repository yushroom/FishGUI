#pragma once

#include <FishGUI/ModelView/ItemView.hpp>
#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Input.hpp>
#include <FishGUI/Draw.hpp>
#include <FishGUI/Theme.hpp>
#include <FishGUI/Utils.hpp>

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

	public:

		TreeWidget(const char* name) : Super(name) {
		}

		virtual void Render() override
		{
			assert(m_model != nullptr);
			Cell(m_root);
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
				//if (e != nullptr && !e->isAccepted())
				//{
				//	bool insidePreicon = PointInRect(e->pos(), preiconRect);
				//	clickedPreicon = insidePreicon && e->type() == MouseEvent::Type::MouseButtonPress;
				//	bool inside = (!insidePreicon) && PointInRect(e->pos(), totalCellRect);
				//	bool clicked = inside && e->type() == MouseEvent::Type::MouseButtonPress;

				//	//if (clicked)
				//	//{
				//	//	m_selectionModel.OnItemClicked(go, e);
				//	//}
				//}

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
				if (m_model->rows(go) > 0)
				{
					//if (clickedPreicon)
					//{
					//	if (isUnfolded)
					//		m_unfolded.erase(it2);
					//	//					m_model.FoldItem(go);
					//	else
					//		m_unfolded.insert(go);
					//	//					m_model.UnfoldItem(go);
					//	e->Accept();
					//}

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

				//auto icon = m_model.icon(go);
				//if (icon != nullptr)
				//{
				//	x += iconWidth / 2 + 3;
				//	nvgFontSize(ctx, (float)icon->fontSize);
				//	nvgFontFace(ctx, icon->fontFace);
				//	nvgFillColor(ctx, theme->textColor);
				//	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
				//	nvgText(ctx, (float)x, (float)y, icon->fontText.c_str(), nullptr);

				//	rect.x += 14;
				//}

				// text
				//				rect.x += 2;
				rect.width -= 20 + 2;
				Label(m_model->text(go), rect);
			}

			Indent(10);
			if (m_model->rows(go) > 0 && isUnfolded)
			{
				for (int i = 0; i < m_model->rows(go); ++i)
				{
					Cell(m_model->childAt(i, 1, go));
				}
			}
			Unindent(10);
		}
	};
}