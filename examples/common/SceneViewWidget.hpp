#pragma once

#include <FishGUI/Widget.hpp>
#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Shader.hpp>
#include <FishGUI/Render/FrameBuffer.hpp>

class SceneViewWidget : public FishGUI::Widget
{
public:

	SceneViewWidget(const char* name);

	void RenderScene();

	void Run();

	void Stop();

	void Pause()
	{
		m_paused = true;
	}

	void Resume()
	{
		m_paused = false;
	}

	void NextFrame()
	{
		//m_timer += m_timePerFrame;
		m_pauseAfterNextFrame = true;
	}

	virtual void Draw() override
	{
		FishGUI::Image(m_frameBuffer.GetColorTexture(), m_rect, false);
	}

protected:
	FishGUI::Shader m_shader;
	unsigned int quadVAO, quadVBO;
	FishGUI::FrameBuffer m_frameBuffer;

	bool m_running = false;
	bool m_pauseAfterNextFrame = false;
	bool m_paused = false;
	//float m_startTime = 0.0f;
	float m_timePerFrame = 1.0f / 30.0f;
	float m_timer = 0.0f;

	bool m_isFirstFrame = true;
};
