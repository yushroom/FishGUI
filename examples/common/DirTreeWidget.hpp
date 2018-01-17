#pragma once

#include <FishGUI/Widget.hpp>
#include <FishGUI/ModelView/ItemView.hpp>
#include "FileNode.hpp"
#include "TreeWidget.hpp"

#include <iostream>


class DirTreeModel : public FishGUI::TTreeModel<FileNode*>
{
	inline FileNode* childAt(int row, int column, FileNode* parent) const override
	{
		return parent->subdirs[row];
	}

	inline int childCount(FileNode* parent) const override
	{
		if (parent == nullptr)
			return 0;
		return static_cast<int>(parent->subdirs.size());
	}

	inline std::string text(FileNode* item) const override
	{
		return item->fileName;
	}

	//inline const FontIcon*	icon(FileNode* item) const
	//{
	//	static char icon[8];
	//	static FontIcon folderIcon = { CodePointToUTF8(0xe930, icon), 12, "ui" };
	//	return &folderIcon;
	//}
};


class DirTreeWidget : public FishGUI::TreeWidget<FileNode*>
{
public:
	typedef FishGUI::TreeWidget<FileNode*> Super;
	
	DirTreeWidget(const char* name, FileNode* root) : Super(name)
	{
		m_model = new DirTreeModel();
		m_selectionModel.SetMode(FishGUI::SelectionMode::SingleSelection);
		m_root = root;
		m_unfolded.insert(m_root);
		m_selectionModel.selectItem(m_root);
	}
};
