#pragma once

#include <ostream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "Input.hpp"
#include "Widget.hpp"

struct NVGcontext;

namespace FishGUI
{
	struct FishGUIContext;
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
		//RenderContext	renderContext;
		//CenterScreen	centerScreen;
		//Input			input;
		//TabState		currentTab;
		//WindowManager	windowManager;
		//Cursor			cursor;
		NVGcontext *	m_nvgContext = nullptr;
	};


	void Init();
	void Run();
}
