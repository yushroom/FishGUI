#pragma once

#include <FishGUI/Widget.hpp>

class UnityToolBar : public FishGUI::ToolBar
{
public:
	UnityToolBar() : ToolBar() {}
	
	UnityToolBar(UnityToolBar&) = delete;
	UnityToolBar& operator=(UnityToolBar&) = delete;
	
	virtual void Draw() override;
	
	void Run();
	void Pause();
	void NextFrame();
	void SetTransformType();
};
