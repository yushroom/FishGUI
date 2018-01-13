#pragma once

#include <string>
#include "Vector.hpp"

//#include "nanovg.h"

struct NVGcontext;

namespace FishGUI
{
	class Window;
	
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
		
	private:
		Context();
		~Context();
		
		Context(Context&) = delete;
		Context& operator=(Context&) = delete;
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
	
	// explicitly set rect
	bool Button(const std::string & text, const Rect& rect);
	void Label(const std::string & text, const Rect& rect);
	
	
	// useful widgets
	void Float3(const std::string & label, float& x, float& y, float& z);
	
	
//	struct SegmentedButton
//	{
//		const char* text;
//		const NVGcolor& fontColor;
//		bool active;
//		bool& outClicked;
//	};
//
//	void SegmentedButtons(int count, SegmentedButton buttons[], const Rect& rect);
}
