#pragma once

#include <FishGUI/Widget.hpp>
#include <functional>

class UnityToolBar : public FishGUI::ToolBar
{
public:
	UnityToolBar() : ToolBar() {}
	
	UnityToolBar(UnityToolBar&) = delete;
	UnityToolBar& operator=(UnityToolBar&) = delete;
	
	virtual void Draw() override;
	
	typedef std::function<void(void)> Callback;

	void SetRunCallback(Callback func) { m_runCallback = func; }
	void SetStopCallback(Callback func) { m_stopCallback = func; }
	void SetPauseCallback(Callback func) { m_puaseCallback = func; }
	void SetResumeCallBack(Callback func) { m_resumeCallback = func; }
	void SetNextFrameCallback(Callback func) { m_nextFrameCallback = func; }
	void SetTransformType();

protected:

	void Run() { if (m_runCallback) m_runCallback(); }
	void Stop() { if (m_stopCallback) m_stopCallback(); }
	void Pause() { if (m_puaseCallback) m_puaseCallback(); }
	void Resume() { if (m_resumeCallback) m_resumeCallback(); }
	void NextFrame() { if (m_nextFrameCallback) m_nextFrameCallback(); }

	Callback m_runCallback;
	Callback m_stopCallback;
	Callback m_puaseCallback;
	Callback m_resumeCallback;
	Callback m_nextFrameCallback;
};
