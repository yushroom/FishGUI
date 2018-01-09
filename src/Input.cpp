#include "Input.hpp"
#include <iostream>

#include "Widget.hpp"

namespace FishGUI
{
	void Input::Update()
	{
		m_leftMouseButtonDoubleClicked = false;
		for (auto& s : m_mouseButtonStates)
		{
			if (s == MouseButtonState::Down)
				s = MouseButtonState::Held;
			else if (s == MouseButtonState::Up)
				s = MouseButtonState::None;
		}

		// dragging
		if (m_mouseButtonStates[0] == MouseButtonState::Held)
		{
			if (m_dragWidget != nullptr)
			{
				m_dragWidget->MouseDragEvent();
			}
		}
		else
		{
			m_dragWidget = nullptr;
		}
		
		// scroll
		m_scrolling = false;
		m_scroll.x = 0;
		m_scroll.y = 0;
	}
}
