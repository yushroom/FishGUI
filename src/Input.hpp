#pragma once
//#include "FishGUI.hpp"
#include <GLFW/glfw3.h>

#include "Vector.hpp"

//struct GLFWcursor;

namespace FishGUI
{
	enum class KeyState
	{
		None = 0,
		Down = 1,
		Held = 2,
		Up = 3,
	};
	
	enum class MouseButtonState
	{
		None = 0,
		Down = 1,
		Held = 2,
		Up = 3,
	};
	
	enum class MouseButton
	{
		Left = 0,
		Right = 1,
		Middle = 2,
	};
	
	class Widget;
	
	class Input
	{
	public:
		static Input& GetInstance()
		{
			static Input input;
			return input;
		}
		
		void Clear()
		{
			for (auto& s : m_mouseButtonStates)
			{
				s = MouseButtonState::None;
			}
			m_inputMode = false;
			m_stringBuffer.clear();
			m_mousePosition.x = 0;
			m_mousePosition.y = 0;
			m_leftMouseButtonDoubleClicked = false;
			
			for (int i = 0; i < 3; ++i)
			{
				m_lastMouseButtonClickTime[i] = 0;
			}
		}
		
		void Update();

		Vector2i GetMousePosition() const
		{
			return m_mousePosition;
		}
		
		// Returns whether the given mouse button is held down.
		// button values are 0 for left button, 1 for right button, 2 for the middle button.
		bool GetMouseButton(MouseButton button)
		{
			return m_mouseButtonStates[static_cast<int>(button)] == MouseButtonState::Held;
		}
		
		// Returns true during the frame the user pressed the given mouse button.
		// button values are 0 for left button, 1 for right button, 2 for the middle button.
		bool GetMouseButtonDown(MouseButton button)
		{
			return m_mouseButtonStates[static_cast<int>(button)] == MouseButtonState::Down;
		}
		
		// Returns true during the frame the user releases the given mouse button.
		// button values are 0 for left button, 1 for right button, 2 for the middle button.
		bool GetMouseButtonUp(MouseButton button)
		{
			return m_mouseButtonStates[static_cast<int>(button)] == MouseButtonState::Up;
		}
		
		MouseButtonState GetMouseButtonState(MouseButton button)
		{
			return m_mouseButtonStates[static_cast<int>(button)];
		}

		bool MouseInRect(int x, int y, int w, int h)
		{
			auto mx = m_mousePosition.x;
			auto my = m_mousePosition.y;
			return PointInRect(mx, my, x, y, w, h);
		}
		
		bool MouseInRect(const Rect & rect)
		{
			return MouseInRect(rect.x, rect.y, rect.width, rect.height);
		}

	private:
		Input()
		{
			Clear();
		}

		Input(Input&) = delete;
		Input& operator=(Input&) = delete;

	public:
		
		MouseButtonState    m_mouseButtonStates[3];
		Vector2i            m_mousePosition = {0, 0};
		bool                m_inputMode = false;
		std::string         m_stringBuffer;
		
		// time record for double-click check
		double              m_lastMouseButtonClickTime[3] = {0, 0, 0};
		
		// TODO
		bool                m_leftMouseButtonDoubleClicked = false;

		Widget * m_dragWidget = nullptr;
	};
	
	enum class CursorType
	{
		Arrow = 0,
		IBeam,
		Crosshair,
		Hand,
		HResize,
		VResize,
		// not a cursor
		CursorCount
	};
	
	class Cursor
	{
	public:

		static Cursor& GetInstance()
		{
			static Cursor cursor;
			return cursor;
		}

		void Init()
		{
			for (int i = 0; i < static_cast<int>(CursorType::CursorCount); ++i) {
				m_glfwCursors[i] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR + i);
			}
		}

		void SetCursorType(CursorType type)
		{
			m_cursorType = type;
		}

		void Draw();

	private:
		Cursor() = default;
		Cursor(Cursor&) = delete;
		Cursor& operator=(Cursor&) = delete;

	private:
		bool		m_cursorChanged = true;
		CursorType	m_cursorType = CursorType::Arrow;
		CursorType	m_cursorTypeLastFame = CursorType::Hand;
		GLFWcursor* m_glfwCursors[(int)CursorType::CursorCount];
	};
}
