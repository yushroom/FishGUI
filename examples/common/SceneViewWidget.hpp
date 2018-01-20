#pragma once

#include <FishGUI/Widget.hpp>
#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Shader.hpp>

class SceneViewWidget : public FishGUI::Widget
{
public:

	SceneViewWidget(const char* name);

	void RenderScene();

	void Pause()
	{

	}

	void Run()
	{

	}

	void NextFrame()
	{

	}

	void ResizeFramebuffer();

	virtual void Draw() override
	{
		FishGUI::Image(textureColorbuffer, m_rect, true);
	}

protected:
	FishGUI::Shader m_shader;
	FishGUI::Size m_framebufferSize;
	unsigned int cubeVAO, cubeVBO;
	unsigned int planeVAO, planeVBO;
	unsigned int m_framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;
};
