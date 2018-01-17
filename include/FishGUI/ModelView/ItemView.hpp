#pragma once

#include <list>
#include <FishGUI/Widget.hpp>
//#include "../common/FileItemModel.hpp"
//#include "../common/TSelectionModel.hpp"

#include <FishGUI/Input.hpp>

#include <GLFW/glfw3.h>

namespace FishGUI
{
	template<class T>
	class TItemModel
	{
		//friend class TItemView<T>;
	public:
		virtual int rows(T item)    const = 0;
		virtual int columns(T item) const = 0;
		virtual T childAt(int row, int column, T parent) const = 0;
		virtual int childCount(T item) const = 0;
		virtual std::string text(T item) const = 0;
	};


	template<class T>
	class TListModel : public TItemModel<T>
	{
	public:
		virtual inline int rows(T item)    const override { return (int)std::ceil(childCount(item) / float(m_columns)); }
		virtual inline int columns(T item = nullptr) const override { return m_columns; }

		// TODO: fast method
		T childAtIndex(int idx, T parent)
		{
			int row = idx / m_columns;
			int col = idx % m_columns;
			return childAt(row, col, parent);
		}

		// internal use only
		void SetColumns(int columns) { m_columns = columns; }

	protected:
		int m_columns = 1;
	};

	template<class T>
	class TTreeModel : public TItemModel<T>
	{
	public:
		virtual inline int rows(T item)    const override { return childCount(item); }
		virtual inline int columns(T item = nullptr) const override { return 1; }

		//// TODO: fast method
		//T childAtIndex(int idx, T parent)
		//{
		//	return childAt(row, 1, parent);
		//}
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

		inline const std::list<T>& selections() const { return m_selection; }

		void ClearSelections()
		{
			m_selection.clear();
			m_lastSelected = m_rangeSelectionBegin = m_rangeSelectionEnd = nullptr;
			SelectionChanged();
		}

		void selectItem(T item, SelectionFlag flag = SelectionFlag::ClearAndSelect)
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

	protected:

		void BlockSignals(bool block)
		{
			m_signalBlocked = block;
		}
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

		// for range selection
		T m_rangeSelectionBegin = nullptr;
		T m_rangeSelectionEnd = nullptr;
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
		void scrollTo(T item)
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

			if (m_selectionModel.selections().empty())
				return;

			if (e->type() != KeyEvent::Type::KeyPress)
			{
				return;
			}

			// first selection
			T first = nullptr;
			int idx = 0;
			first = m_selectionModel.selections().front();
			//for (auto w : m_visibleItems)
			//{
			//	if (m_selectionModel.IsSelected(w))
			//	{
			//		first = w;
			//		break;
			//	}
			//	idx++;
			//}
			auto it = std::find(m_visibleItems.begin(), m_visibleItems.end(), first);
			idx = (int)std::distance(m_visibleItems.begin(), it);

			auto key = e->key();
			auto mod = e->modifiers();

			int offset = 0;

			// TODO: remove nullptr
			int columns = m_model->columns(nullptr);	// items per row
			if (columns == 1)		// 1D
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
					offset = -columns;
				else if (key == GLFW_KEY_DOWN)
					offset = columns;
			}

			idx += offset;
			if (idx >= 0 && idx < m_visibleItems.size())
			{
				//if (idx >= m_visibleItems.size())
				//	idx = m_visibleItems.size() - 1;
	//			auto rect = m_visibleItemRects[idx];
				auto& item = m_visibleItems[idx];
				m_selectionModel.selectItem(item, SelectionFlag::ClearAndSelect);

				__ScrollTo(idx);
			}
		}

		void OnItemClicked(T item, MouseEvent* e)
		{
			assert(e != nullptr);
			//auto it = m_selected.find(item);
			//bool selected = (it != m_selected.end());
			bool selected = m_selectionModel.IsSelected(item);
			bool isMulti = (m_selectionModel.GetMode() == SelectionMode::Extended);
			int mods = e->modifiers();
#if __APPLE__
			constexpr int MODIFIER = int(Modifier::Super);
#else
			constexpr int MODIFIER = int(Modifier::Ctrl);
#endif
			bool appendMode = isMulti && (e->modifiers() & MODIFIER) != 0;
			//bool rangeMode = isMulti &&
			//	(e->modifiers() & int(Modifier::Shift)) != 0 &&
			//	m_lastSelected != nullptr;
			bool rangeMode = false;

			SelectionFlag flag;
			if (appendMode)
			{
				if (selected)
					flag = SelectionFlag::Clear;
				else
					flag = SelectionFlag::Select;
				//if (selected)
				//	m_selected.erase(it);
				//else
				//	Select(item);
			}
			else if (rangeMode)
			{
				//m_rangeSelectionBegin = m_lastSelected;
				//m_rangeSelectionEnd = item;
			}
			else	// single selection
			{
				//ClearSelections();
				//Select(item);
				flag = SelectionFlag::ClearAndSelect;
			}
			m_selectionModel.selectItem(item, flag);

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
						break;
					}
				}
				if (idx >= 0)
				{
					auto itemClicked = m_visibleItems[idx];
					OnItemClicked(itemClicked, e);
					__ScrollTo(idx);
				}

				/*
				if (m_selectionModel.mode() == SelectionMode::Extended && m_rangeSelectionEnd != nullptr)
				{
					//			m_lastSelected = m_rangeSelectionBegin;
					auto it1 = std::find(m_visibleItems.begin(), m_visibleItems.end(), m_rangeSelectionBegin);
					auto it2 = std::find(m_visibleItems.begin(), m_visibleItems.end(), m_rangeSelectionEnd);

					if (it1 == m_visibleItems.end())
					{
					}
					else if (it2 == m_visibleItems.end())
					{
					}
					else
					{
						ClearSelections();
						if (it1 > it2)
							std::swap(it1, it2);
						it2++;
						for (auto it = it1; it != it2; ++it)
						{
							m_selected.insert(*it);
						}
						m_lastSelected = m_rangeSelectionBegin = *it1;
					}
					m_rangeSelectionEnd = nullptr;
				}
				*/
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
	};
}