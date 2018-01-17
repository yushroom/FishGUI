#include "FileListWidget.hpp"

#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Draw.hpp>
#include <FishGUI/Input.hpp>
#include <FishGUI/Theme.hpp>

using namespace FishGUI;


FileListWidget::FileListWidget(const char* name) : Super(name)
{
	m_model = new FileListModel();
}

