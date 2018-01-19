#pragma once

#include <string>
#include "Math.hpp"

struct NVGcontext;

namespace FishGUI
{
	enum class DockPosition
	{
		Left,
		Right,
		Top,
		Bottom,
		Center,
	};

	struct FishGUIContext
	{
		NVGcontext *	m_nvgContext = nullptr;
	};
	
	struct DrawContext;
	class Window;
	class Widget;
	class Input;
	
	struct Context
	{
		DrawContext*	m_drawContext = nullptr;
		Input*			m_input = nullptr;
		
		static Context& GetInstance()
		{
			static Context instance;
			return instance;
		}
		
		void BindWindow(Window* window);
		void BindWidget(Widget* widget);
		void UnbindWindow();
		void UnbindWidget();
		
	private:
		// make it singleton
		Context();
		~Context();
		Context(Context&) = delete;
		Context& operator=(Context&) = delete;
		
		Window*			m_window = nullptr;	// current window;
		Widget*			m_widget = nullptr; // current widget;
	};
	
	Window* NewWindow(const char* title);

	void Init();
	void Run();
	
	Rect NewLine(int height);
	
	void Group(const std::string & name);
	void EndGroup();
	
	void Indent(int indent_w);
	void Unindent(int indent_w);
	
	// basic imgui widgets
	// auto layout version, can only be used in a IMWidget::Draw
	bool Button(const std::string & text);
	void Label(const std::string & text);
	bool CheckBox(const std::string & label, bool& inoutValue);
	void InputText(const std::string & label, std::string& inoutValue);
	bool Slider(const std::string & label, float& inoutValue, float minValue, float maxValue);
	void Combox(const std::string & label, const std::string& inoutValue);
	
	void Image(unsigned int image, int width, int height, bool flip = false);
	
	// explicitly set rect
	bool Button(const std::string & text, const Rect& rect);
	void Label(const std::string & text, const Rect& rect);
	
	// set flip=true when drawing framebuffer
	void Image(unsigned int image, const Rect& r, bool flip = false);
	
	// useful widgets
	void Float3(const std::string & label, float& x, float& y, float& z);
}

void _checkOpenGLError(const char *file, int line);
#define glCheckError() _checkOpenGLError(__FILE__,__LINE__);
