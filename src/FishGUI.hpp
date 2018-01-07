#pragma once

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
}
