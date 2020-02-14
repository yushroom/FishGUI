#pragma once
#include <string>
#include <vector>
#include <cassert>
#include <functional>

#include "Math.hpp"

namespace FishGUI
{
	enum class Alignment : uint8_t
	{
		Minimum = 0,	// Take only as much space as is required.
		Middle,			// Center align.
		Maximum,		// Take as much space as is allowed.
		Fill			// Fill according to preferred sizes.
	};

	enum class Orientation
	{
		Horizontal = 0,
		Vertical,
	};

	struct FishGUIContext;
	struct Theme;
	class Layout;
	struct KeyEvent;
	struct MouseEvent;

	class Widget
	{
	public:
		Widget(const char* name = "") : m_name(name)
		{
		}

		// non copyable
		Widget(const Widget&) = delete;
		const Widget& operator=(const Widget&) = delete;

		virtual ~Widget() = default;

		const std::string& GetName() const { return m_name; }
		void SetName(const std::string& name) { m_name = name; }

		Theme* GetTheme() { return m_theme; }
		void SetTheme(Theme* theme) { m_theme = theme; }

		int	GetWidth() const { return m_rect.width; }
		int	GetHeight() const { return m_rect.height; }
		void SetWidth(int w) { m_rect.width = w; }
		void SetHeight(int h) { m_rect.height = h; }

		void SetMinSize(int w, int h) { m_minSize.width = w; m_minSize.height = h; }

		Layout* GetLayout() { return m_layout; }
		void SetLayout(Layout* layout);

		const Rect & GetRect() const { return m_rect; }
		void SetRect(int x, int y, int w, int h)
		{
			m_rect.x = x;
			m_rect.y = y;
			m_rect.width = w;
			m_rect.height = h;
		}

		bool IsFocused() const { return m_isFocused; }
		void SetIsFocused(bool focused) { m_isFocused = focused; }

		virtual void Draw();
		void BindAndDraw();

		void OnKeyEvent(KeyEvent* e) { m_keyEvent = e; };
		void OnMouseEvent(MouseEvent* e) { m_mouseEvent = e; };

		//virtual bool MouseMotionEvent(const Vector2i & p, const Vector2i & rel, int button, int modifiers)
		//{
		//	return false;
		//}

		// virtual bool MouseDragEvent(const Vector2i & p, const Vector2i & rel, int button, int modifiers)
		virtual bool MouseDragEvent(const Vector2i & mousePos)
		{
			return false;
		}

	protected:
		friend class Layout;
		friend class SimpleLayout;
		friend class SplitLayout;
		friend class HLayout;
		friend class VLayout;

		Theme*				m_theme = nullptr;
		Layout * 			m_layout = nullptr;
		std::string 		m_name;
		Rect 				m_rect = {0, 0, 16, 16};
		Size				m_fixedSize = { -1, -1 };
		Size				m_minSize = { 1, 1 };
		Size				m_maxSize = { 4096, 4096 };

		// a focused widget is the first responder of keyboard event
		bool				m_isFocused = false;

		KeyEvent*	m_keyEvent = nullptr;
		MouseEvent* m_mouseEvent = nullptr;
	};


	class Layout
	{
	public:
		Layout() = default;
		Layout(Layout&) = delete;
		Layout& operator=(Layout&) = delete;

		virtual void PerformLayout(const Rect& rect) = 0;
		virtual ~Layout() = default;
	};


	class SimpleLayout : public Layout
	{
	public:
		virtual void PerformLayout(const Rect& rect) override;
		void SetWidget(Widget* widget) { m_widget = widget; }
		Widget* GetWidget() { return m_widget; }

	protected:
		Widget* m_widget;
	};


	class Splitter : public Widget
	{
	public:
		static constexpr int Width = 3;

		Splitter(Orientation orientation)
			: Widget("Splitter"), m_orientation(orientation)
		{
		}

		virtual bool MouseDragEvent(const Vector2i & mousePos) override;

		void Draw3() const;

	private:
		friend class Layout;
		friend class SplitLayout;
		Orientation m_orientation;
		bool m_dirty = false;
		int m_pos = 0;
	};

	class SplitLayout : public Layout
	{
	public:
		SplitLayout(Orientation orientation)
			: m_orientation(orientation), m_splitter(orientation)
		{
		}

		virtual void PerformLayout(const Rect& rect) override;

		static constexpr int INTERVAL = Splitter::Width;

		enum class PartType
		{
			EWidget,
			ELayout,
		};

		struct Part
		{
			union
			{
				Widget * w;
				SplitLayout * l;
			};
			PartType type;

			Size GetMinSize()
			{
				if (type == PartType::ELayout)
					return l->m_minSize;
				else
					return w->m_minSize;
			}
		};

		void Split(Widget* w1, Widget* w2)
		{
			assert(!m_done);
			part1.w = w1;
			part2.w = w2;
			part1.type = PartType::EWidget;
			part2.type = PartType::EWidget;
			m_done = true;
		}

		void Split(Widget* w, SplitLayout* l)
		{
			assert(!m_done);
			part1.w = w;
			part2.l = l;
			part1.type = PartType::EWidget;
			part2.type = PartType::ELayout;
			m_done = true;
		}

		void Split(SplitLayout* l, Widget* w)
		{
			assert(!m_done);
			part1.l = l;
			part2.w = w;
			part1.type = PartType::ELayout;
			part2.type = PartType::EWidget;
			m_done = true;
		}

		void Split(SplitLayout* l1, SplitLayout* l2)
		{
			assert(!m_done);
			part1.l = l1;
			part2.l = l2;
			part1.type = PartType::ELayout;
			part2.type = PartType::ELayout;
			m_done = true;
		}

	protected:
		friend class Widget;
		Orientation m_orientation;
		Splitter m_splitter;
		bool m_done = false;

		Size m_minSize = {1, 1};

		Part part1;
		Part part2;
	};

	typedef std::function<void(void)> RenderFunction;


	// abstract super class for tool bar
	class ToolBar : public Widget
	{
	public:
		ToolBar() : Widget("ToolBar")
		{
			m_rect.height = 32;
		}

		ToolBar(ToolBar&) = delete;
		ToolBar& operator=(ToolBar&) = delete;
	};


	class StatusBar : public Widget
	{
	public:
		StatusBar() : Widget("StatusBar")
		{
			m_rect.height = 20;
		}

		StatusBar(StatusBar&) = delete;
		StatusBar& operator=(StatusBar&) = delete;

		virtual void Draw() override;

		void SetText(const std::string& text)
		{
			m_text = text;
		}

	protected:
		std::string m_text;
	};


	class TabWidget : public Widget
	{
	public:
		int m_activeTabId = 0;

		TabWidget(const char* name) : Widget(name)
		{
		}

		TabWidget(TabWidget&) = delete;
		TabWidget& operator=(TabWidget&) = delete;


		void AddChild(Widget* w)
		{
			m_children.push_back(w);
		}

		virtual void Draw() override;

		void SetActiveTab(int index)
		{
			assert(index >= 0 && index < m_children.size());
			m_activeTabId = index;
		}

	protected:
		std::vector<Widget*> m_children;
	};


	struct IMGUIContext;

	// IMGUI widget with auto layout
	class IMWidget : public Widget
	{
	public:
		IMWidget(const char* name);
		virtual ~IMWidget();

		IMWidget(IMWidget&) = delete;
		IMWidget& operator=(IMWidget&) = delete;

		virtual void Draw() override;

		virtual void DrawImpl() = 0;

	protected:
		IMGUIContext*	m_imContext;
	};


	struct IMGUIContext
	{
		IMWidget* 		widget = nullptr;
		Rect 			rect = {0, 0, 1, 1};
		Vector2i 		pos;			// start position of the next cell
		float 			yStart = 0;
		int				totalHeight = 0;	// total height of last frame
		static constexpr int cellHeight = 16;
		static constexpr int xmargin = 2;
		int ymargin = 2;
		static constexpr int Indent = 10;

		// scroll bar status depends on last frame status
		bool showScrollBar = false;
		bool needScrollBar = false;
		static constexpr int scrollBarWidth = 10;

		int indent = 0;

		void Reset()
		{
			yStart = 0;
			pos.x = rect.x;
			pos.y = rect.y;
			indent = 0;
		}

		void BeginFrame()
		{
			if (rect.height - totalHeight > 0)
			{
				yStart = 0;
			}

			indent = 0;
			rect = widget->GetRect();
			pos.x = rect.x;
			pos.y = static_cast<int>( rect.y + yStart );
			showScrollBar = needScrollBar;	// set by last frame
			needScrollBar = false;			// clear for this frame
		}

		int Right()
		{
			int r = rect.x + rect.width;
			if (showScrollBar)
				r -= scrollBarWidth;
			return r;
		}

		void EndFrame();
		Rect NextCell(int height, bool& outOfRange);
		// label + ...
		void NextCell2(Rect& left, Rect& right, bool& outOfRange, float leftLen = 1, float rightLen = 1, int height = cellHeight);
		void AddIndent(int indent = Indent);
		void HLine();
	};
}
