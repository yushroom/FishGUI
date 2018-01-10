#pragma once

#include <string>
#include "Vector.hpp"

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
	void CheckBox(const std::string & label, bool& inoutValue);
	void InputText(const std::string & label, std::string& inoutValue);
	bool Slider(const std::string & label, float& inoutValue, float minValue, float maxValue);
	void Combox(const std::string & label, const std::string& inoutValue);
	
	// explicitly set rect
	bool Button(const std::string & text, const Rect& rect);
	void Label(const std::string & text, const Rect& rect);
	
	
	// useful widgets
	void Float3(const std::string & label, float& x, float& y, float& z);
	
	void SegmentedButtons(int count, const char* labels[], int icons[], const Rect& rect);
}
