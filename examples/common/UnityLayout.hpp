#pragma once

#include <FishGUI/Widget.hpp>


class UnityLayout : public FishGUI::Layout
{
public:
	virtual void PerformLayout(const FishGUI::Rect& rect) override
	{
		auto r = rect;
		if (m_toolBar != nullptr)
		{
			m_toolBar->SetRect(0, 0, r.width, m_toolBar->GetHeight());
			m_toolBar->BindAndDraw();
		}

		if (m_statusBar != nullptr)
		{
			m_statusBar->SetRect(0, r.height - m_statusBar->GetHeight(), r.width, m_statusBar->GetHeight());
			m_statusBar->BindAndDraw();
		}
		if (m_toolBar != nullptr)
		{
			r.height -= m_toolBar->GetHeight();
			r.y += m_toolBar->GetHeight();
		}
		if (m_statusBar != nullptr)
		{
			r.height -= m_statusBar->GetHeight();
		}

		if (m_centerLayout != nullptr)
		{
			m_centerLayout->PerformLayout(r);
		}
	}

	void SetCenterLayout(Layout* layout)
	{
		m_centerLayout = layout;
	}

	void SetToolBar(FishGUI::ToolBar* toolBar)
	{
		m_toolBar = toolBar;
	}

	void SetStatusBar(FishGUI::StatusBar* statusBar)
	{
		m_statusBar = statusBar;
	}

	FishGUI::ToolBar* GetToolBar()
	{
		return m_toolBar;
	}

	FishGUI::StatusBar* GetStatusBar()
	{
		return m_statusBar;
	}

protected:
	FishGUI::ToolBar* 	m_toolBar 	= nullptr;
	FishGUI::StatusBar* 	m_statusBar = nullptr;
	Layout* 	m_centerLayout = nullptr;
};
