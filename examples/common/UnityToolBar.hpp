#pragma once

#include <FishGUI/Widget.hpp>
#include <functional>
#include <boost/signals2/signal.hpp>

class UnityToolBar : public FishGUI::ToolBar
{
public:
	UnityToolBar() : ToolBar() {}
	
	UnityToolBar(UnityToolBar&) = delete;
	UnityToolBar& operator=(UnityToolBar&) = delete;
	
	virtual void Draw() override;
	
	typedef std::function<void(void)> Callback;

	void SetTransformType();

	boost::signals2::signal<void(void)> OnRun;
	boost::signals2::signal<void(void)> OnStop;
	boost::signals2::signal<void(void)> OnPause;
	boost::signals2::signal<void(void)> OnResume;
	boost::signals2::signal<void(void)> OnNextFrame;

protected:

	void Run() { OnRun(); }
	void Stop() { OnStop(); }
	void Pause() { OnPause(); }
	void Resume() { OnResume(); }
	void NextFrame() { OnNextFrame(); }
};
