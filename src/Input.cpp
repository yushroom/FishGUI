#include <FishGUI/Input.hpp>

#include <iostream>

#include <GLFW/glfw3.h>

#include <FishGUI/Widget.hpp>

namespace FishGUI
{
	Input* 	Input::s_current = nullptr;
	
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
				m_dragWidget->MouseDragEvent(m_mousePosition);
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
	
	void Cursor::Init()
	{
		for (int i = 0; i < static_cast<int>(CursorType::CursorCount); ++i) {
			m_glfwCursors[i] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR + i);
		}
	}
}
