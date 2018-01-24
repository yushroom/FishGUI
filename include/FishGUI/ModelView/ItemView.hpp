#pragma once

#include <list>
#include <algorithm>
#include <GLFW/glfw3.h>

#include "../Widget.hpp"
#include "../Input.hpp"
#include "../Icon.hpp"

namespace FishGUI
{
	template<class T>
	class TItemModel
	{
	public:
		virtual T Parent(T child) const = 0;
		virtual T ChildAt(int index, T parent) const = 0;
		virtual int ChildCount(T item) const = 0;
		virtual std::string Text(T item) const = 0;
		virtual const FishGUI::Icon* Icon(T item) const { return nullptr; }
	};

	
	enum class SelectionMode
	{
		//NoSelection,
		SingleSelection,
		//MultiSelection,
		//ContiguousSelection,
		Extended,
	};

	enum class SelectionFlag
	{
		NoUpdate = 0,
		Clear = 0x0001,
		Select = 0x0002,
		Deselect = 0x0004,
		//Toggle = 0x0008,
		//Current = 0x0010,
		ClearAndSelect = Clear | Select,
	};

	template<class T>
	class TItemSelectionModel
	{
		//	friend class TItemView<T>;
	public:

		inline const std::list<T>& GetSelections() const { return m_selection; }

		T CurrentSelected() const
		{
			return m_lastSelected;
		}
		
		void ClearSelections()
		{
			m_selection.clear();
			m_lastSelected = nullptr;
			SelectionChanged();
		}

		void SelectItem(T item, SelectionFlag flag = SelectionFlag::ClearAndSelect)
		{
			if (flag == SelectionFlag::ClearAndSelect)
			{
				BlockSignals(true);
				ClearSelections();
				m_selection.push_back(item);
				BlockSignals(false);
				m_lastSelected = item;
				SelectionChanged();
			}
			else if (flag == SelectionFlag::Clear)
			{
				m_selection.remove(item);
				if (m_selection.empty())
					m_lastSelected = nullptr;
				else
					m_lastSelected = m_selection.back();
				SelectionChanged();
			}
			else if (flag == SelectionFlag::Select)
			{
				m_selection.emplace_back(item);
				m_lastSelected = item;
				SelectionChanged();
			}
			else
			{
				abort();
			}
		}

		bool IsSelected(T item)
		{
			auto it = std::find(m_selection.begin(), m_selection.end(), item);
			return it != m_selection.end();
		}

		void SetMode(SelectionMode mode) { m_mode = mode; }
		SelectionMode GetMode() const { return m_mode; }

		typedef std::function<void(T)> SelectionChangedCallback;

		void SetSelectionChangedCallback(SelectionChangedCallback callback)
		{
			m_onSelectionChanged = callback;
		}

		void BlockSignals(bool block)
		{
			m_signalBlocked = block;
		}

	protected:
		bool m_signalBlocked = false;

		void SelectionChanged()
		{
			if (m_signalBlocked)
				return;
			if (m_onSelectionChanged)
			{
				m_onSelectionChanged(m_lastSelected);
			}
		}


		std::list<T> m_selection;
		SelectionMode m_mode = SelectionMode::Extended;
		SelectionChangedCallback m_onSelectionChanged;

		
		T m_lastSelected = nullptr;
	};


	template<class T>
	class TItemView : public IMWidget
	{
	public:
		typedef TItemModel<T> ModelType;
		typedef TItemSelectionModel<T> SelectionModelType;

		TItemView(const char* name) : IMWidget(name) {}

		void SetModel(ModelType* model)
		{
			m_model = model;
		}

		inline ModelType* GetModel() const { return m_model; }
		inline SelectionModelType* GetSelectionModel() { return &m_selectionModel; }

		void AppendVisibleItem(T item, const Rect& r)
		{
			m_visibleItems.push_back(item);
			m_visibleItemRects.push_back(r);
		}

		// Scrolls the view if necessary to ensure that the item at index is visible.
		void ScrollTo(T item)
		{
			auto it = std::find(m_visibleItems.begin(), m_visibleItems.end(), item);
			assert(it != m_visibleItems.end());
			int idx = std::distance(m_visibleItems.begin(), it);

			__ScrollTo(idx);
		}

		void HandleKeyEvent()
		{
			auto e = m_keyEvent;
			if (e == nullptr || e->isAccepted())
				return;

			if (m_selectionModel.GetSelections().empty())
				return;

			if (e->type() != KeyEvent::Type::KeyPress)
			{
				return;
			}
			
			if (m_selectionModel.GetSelections().empty())
			{
				return;
			}

			auto key = e->key();
//			auto mod = e->modifiers();
			int offset = 0;
			if (m_columns == 1)		// 1D
			{
				if (key == GLFW_KEY_UP)
					offset = -1;
				else if (key == GLFW_KEY_DOWN)
					offset = 1;
			}
			else	// 2D
			{
				if (key == GLFW_KEY_LEFT)
					offset = -1;
				else if (key == GLFW_KEY_RIGHT)
					offset = 1;
				else if (key == GLFW_KEY_UP)
					offset = -m_columns;
				else if (key == GLFW_KEY_DOWN)
					offset = m_columns;
			}

			if (offset == 0)	// key not matched
			{
				return;
			}

			// last selection
			auto last = m_selectionModel.GetSelections().back();;
			int idx = 0;
			auto it = std::find(m_visibleItems.begin(), m_visibleItems.end(), last);
			idx = (int)std::distance(m_visibleItems.begin(), it);

			idx += offset;
			if (idx >= 0 && idx < m_visibleItems.size())
			{
				//if (idx >= m_visibleItems.size())
				//	idx = m_visibleItems.size() - 1;
	//			auto rect = m_visibleItemRects[idx];
				auto& item = m_visibleItems[idx];
				m_selectionModel.SelectItem(item, SelectionFlag::ClearAndSelect);

				__ScrollTo(idx);
			}
			e->Accept();
		}

		void OnItemClicked(T item, MouseEvent* e)
		{
			assert(item != nullptr && e != nullptr);
			//auto it = m_selected.find(item);
			//bool selected = (it != m_selected.end());
			bool selected = m_selectionModel.IsSelected(item);
			bool isMulti = (m_selectionModel.GetMode() == SelectionMode::Extended);
//			int mods = e->modifiers();
#if __APPLE__
			constexpr int MODIFIER = int(Modifier::Super);
#else
			constexpr int MODIFIER = int(Modifier::Ctrl);
#endif
			auto lastSelected = m_selectionModel.CurrentSelected();

			bool appendMode = isMulti && (e->modifiers() & MODIFIER) != 0;
			bool rangeMode = isMulti && (e->modifiers() & int(Modifier::Shift)) != 0 && lastSelected != nullptr;
			
			if (appendMode)
			{
				SelectionFlag flag;
				if (selected)
					flag = SelectionFlag::Clear;
				else
					flag = SelectionFlag::Select;
				m_selectionModel.SelectItem(item, flag);
			}
			else if (rangeMode)
			{
				auto rangeSelectionBegin = lastSelected;
				auto rangeSelectionEnd = item;

				auto it1 = std::find(m_visibleItems.begin(), m_visibleItems.end(), rangeSelectionBegin);
				auto it2 = std::find(m_visibleItems.begin(), m_visibleItems.end(), rangeSelectionEnd);

				if (it1 == m_visibleItems.end())
				{
				}
				else if (it2 == m_visibleItems.end())
				{
				}
				else
				{
					m_selectionModel.BlockSignals(true);
					m_selectionModel.ClearSelections();
					//int step = 1;
					if (it1 > it2)
					{
						std::swap(it1, it2);
						//step = -1;
					}
					it2++;
					for (auto it = it1; it != it2; ++it)
					{
						m_selectionModel.SelectItem(*it, SelectionFlag::Select);
					}
					m_selectionModel.BlockSignals(false);
					//m_lastSelected = m_rangeSelectionBegin = *it1;
				}
				//rangeSelectionEnd = nullptr;
			}
			else	// single selection
			{
				m_selectionModel.SelectItem(item, SelectionFlag::ClearAndSelect);
			}

			e->Accept();
		}

		void HandleMouseEvent()
		{
			auto e = m_mouseEvent;
			if (e == nullptr || e->isAccepted())
				return;
			
			if (e->type() == MouseEvent::Type::MouseButtonPress)
			{
				auto p = e->pos();
				int idx = -1;
				for (auto r : m_visibleItemRects)
				{
					idx++;
					if (PointInRect(p, r))
					{
						auto itemClicked = m_visibleItems[idx];
						OnItemClicked(itemClicked, e);
						__ScrollTo(idx);
						break;
					}
				}
			}

			if (!e->isAccepted() && e->button() == MouseButton::Left && e->type() == MouseEvent::Type::MouseButtonPress)
			{
				m_selectionModel.ClearSelections();
				return;
			}
		}

		virtual void DrawImpl() final override
		{
			m_visibleItems.clear();
			m_visibleItemRects.clear();
			Render();
			HandleKeyEvent();
			HandleMouseEvent();
		}

		virtual void Render() = 0;

	protected:

		// idx: item index in m_visibleItems
		void __ScrollTo(int idx)
		{
			auto r = m_visibleItemRects[idx];
			if (r.y < m_rect.top())
			{
				m_imContext->yStart += m_rect.top() - r.y;
			}
			else if (r.bottom() > m_rect.bottom())
			{
				m_imContext->yStart -= r.bottom() - m_rect.bottom();
			}
		}


		TItemModel<T>*			m_model;
		TItemSelectionModel<T> 	m_selectionModel;
		std::vector<T> 			m_visibleItems;
		std::vector<Rect>		m_visibleItemRects;
		
		int m_columns = 1;	// items per row
	};
}
