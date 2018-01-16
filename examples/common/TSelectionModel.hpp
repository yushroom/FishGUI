#pragma once

#include <cassert>
#include <vector>
#include <set>
#include <algorithm>

#include <FishGUI/Widget.hpp>
#include <FishGUI/Input.hpp>

enum class SelectionType
{
	None,
	Single,
	Multi,
};

template<class T>
class TSelectionModel
{
public:
	TSelectionModel() = default;
	TSelectionModel(TSelectionModel&) = delete;
	TSelectionModel& operator=(TSelectionModel&) = delete;

	bool IsSelected(T item)
	{
		bool selected = (m_selected.find(item) != m_selected.end());
		return selected;
	}
	
	void Select(T item)
	{
		m_selected.insert(item);
		m_lastSelected = item;
		m_rangeSelectionEnd = nullptr;
		SelectionShanged();
	}
	
	void OnItemClicked(T item, FishGUI::MouseEvent* e)
	{
		assert(e != nullptr);
		auto it = m_selected.find(item);
		bool selected = (it != m_selected.end());
		bool isMulti = (m_type == SelectionType::Multi);
		int mods = e->modifiers();
#if __APPLE__
		constexpr int MODIFIER = int(FishGUI::Modifier::Super);
#else
		constexpr int MODIFIER = int(FishGUI::Modifier::Ctrl);
#endif
		bool appendMode = isMulti && (e->modifiers() & MODIFIER) != 0;
		bool rangeMode = isMulti &&
			(e->modifiers() & int(FishGUI::Modifier::Shift)) != 0 &&
			m_lastSelected != nullptr;
		
		if (appendMode)
		{
			if (selected)
				m_selected.erase(it);
			else
				Select(item);
//			selected = !selected;
		}
		else if (rangeMode)
		{
			m_rangeSelectionBegin = m_lastSelected;
			m_rangeSelectionEnd = item;
		}
		else	// single selection
		{
			ClearSelections();
			Select(item);
//			selected = true;
		}
		e->Accept();
	}
	
	void BeforeFrame()
	{
		m_visibleItems.clear();
	}
	
	void AfterFrame(FishGUI::MouseEvent* e)
	{
		if (e == nullptr)
			return;
		
//		if (e->type()==FishGUI::MouseEvent::Type::MouseButtonPress)
//		{
//
//		}
		
		if (e->type() == FishGUI::MouseEvent::Type::MouseButtonPress)
		{
			if (!e->isAccepted())
			{
				ClearSelections();
				return;
			}
		
			if (m_type == SelectionType::Multi && m_rangeSelectionEnd != nullptr)
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
		}
	}
	
	void ClearSelections()
	{
		m_selected.clear();
		m_lastSelected = m_rangeSelectionBegin = m_rangeSelectionEnd = nullptr;
	}
	
	void SetSelectionType(SelectionType type)
	{
		m_type = type;
	}
	
	void AppendVisibleItem(T item)
	{
		m_visibleItems.push_back(item);
	}
	
	//	typedef std::function<void(const std::vector<T*>&)> SelectionChangedCallback;
	//	void OnSelectionChanged();
	
	typedef std::function<void(T)> SelectionChangedCallback;
	void SetSelectionChangedCallback(SelectionChangedCallback callback)
	{
		m_onSelectionChanged = callback;
	}
	
private:
	void SelectionShanged()
	{
		if (m_onSelectionChanged != nullptr)
			m_onSelectionChanged(m_lastSelected);
	}
	
	SelectionChangedCallback m_onSelectionChanged;
	
private:
	SelectionType m_type = SelectionType::Single;
	
	std::set<T> m_selected;
	// logically visible, not visually
	std::vector<T> m_visibleItems;
	
	// for range selection
	T m_lastSelected = nullptr;
	T m_rangeSelectionBegin = nullptr;
	T m_rangeSelectionEnd = nullptr;
};
