#pragma once

#include <FishGUI/Widget.hpp>
#include "TSelectionModel.hpp"
#include "FileItemModel.hpp"
#include "TreeViewWidget.hpp"

#include <iostream>

class DirTreeWidget : public FishGUI::TreeWidget<FileNode*>
{
public:
	typedef FishGUI::TreeWidget<FileNode*> Super;
	
	DirTreeWidget(const char* name, FileNode* root) : Super(name)
	{
		m_selectionModel.SetSelectionType(SelectionType::Single);
		m_root = root;
		m_model.SetRootItem(m_root);
		m_unfolded.insert(m_root);
		m_selectionModel.Select(m_root);
	}
};
