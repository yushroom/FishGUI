#pragma once

#include "ItemView.hpp"
#include "../FishGUI.hpp"
#include "../Input.hpp"
#include "../Draw.hpp"
#include "../Theme.hpp"
#include "../Utils.hpp"

namespace FishGUI
{
	template<class T>
	class ListWidget : public TItemView<T>
	{
	protected:
		typedef TItemView<T> Super;
		using Super::m_model;
		using Super::m_selectionModel;
		using Super::AppendVisibleItem;
		using IMWidget::m_imContext;
		using Widget::m_rect;
		using Widget::m_isFocused;
		
	public:
		ListWidget(const char* name) : Super(name) { }

		void SetRoot(T root)
		{
			m_root = root;
			m_imContext->Reset();
			m_selectionModel.ClearSelections();
		}

		virtual void Render() override
		{
			//m_selectionModel.BeforeFrame();
			assert(m_model != nullptr);

			//auto model = dynamic_cast<FileListModel*>(m_model);

			constexpr int imageSize = 64;
			constexpr int textHight = 14;
			constexpr int pad = 6;
			int count = m_model->ChildCount(m_root);

			if (count == 0)
				return;

			auto ctx = Context::GetInstance().m_drawContext;
			auto theme = ctx->theme;

			// rect of each cell (including padding)
			auto r = m_rect;
			r.x = m_rect.x;
			r.y = static_cast<int>(m_rect.y + m_imContext->yStart);
			r.width = imageSize + pad * 2;
			r.height = pad + imageSize + textHight + pad;

			const float totalWidth = float(m_imContext->Right() - m_rect.x);
			const int columns = (int)std::floor(totalWidth / r.width);
			this->m_columns = columns;
			const int rows = (int)std::ceil(float(count)/columns);

//			for (int i = 0; i < count; ++i)
//			{
//				int row = i / columns;
//				int col = i % columns;
//			}

			for (int row = 0; row < rows; ++row)
			{
				bool outOfRange = false;
				m_imContext->NextCell(r.height, outOfRange);

				for (int col = 0; col < columns; ++col)
				{
					int index = row * columns + col;
					if (index >= count)
						break;

					r.x = m_rect.x + r.width * col;
					auto node = m_model->ChildAt(index, m_root);
					AppendVisibleItem(node, r);
					if (!outOfRange)
					{
						//if (m_mouseEvent != nullptr && !m_mouseEvent->isAccepted())
						//{
						//	auto e = m_mouseEvent;
						//	bool inside = PointInRect(e->pos(), r);
						//	bool clicked = inside && e->type() == MouseEvent::Type::MouseButtonPress;

						//	//if (clicked)
						//	//	m_selectionModel.OnItemClicked(node, e);
						//}

						if (m_selectionModel.IsSelected(node))
						{
							auto& color = m_isFocused ? theme->selectionHighlightColor : theme->selectionColor;
							DrawRect(ctx, r, color);
						}


						auto icon = m_model->Icon(node);
						auto imageRect = r;
						imageRect.x += pad;
						imageRect.y += pad;
						imageRect.width = imageSize;
						imageRect.height = imageSize;
						FishGUI::DrawRect(ctx, imageRect, nvgRGB(255, 255, 255));
						//icon->Draw(Context::GetInstance().m_drawContext, imageRect);

						const auto& text = m_model->Text(node);
						auto textRect = r;
						textRect.x += pad;
						textRect.y += pad + imageSize;
						textRect.width = imageSize;
						textRect.height = textHight;
						FishGUI::Label(text, textRect);
					}
				}
				r.y += r.height;
			}
		}

	protected:
		T m_root = nullptr;
	};
}
