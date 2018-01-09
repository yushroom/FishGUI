#pragma once

#include <string>

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
		//Theme			theme;
		NVGcontext *	m_nvgContext = nullptr;
	};
	
	Window* NewWindow(const char* title);

	void Init();
	void Run();
	
	void Group(const std::string & name);
	void EndGroup();
	
	bool Button(const std::string & text);
	void Label(const std::string & text);
	void CheckBox(const std::string & label, bool& inoutValue);
	void InputText(const std::string & label, std::string& inoutValue);
	bool Slider(const std::string & label, float& inoutValue, float minValue, float maxValue);
	
	void Float3(const std::string & label, float& x, float& y, float& z);
}
