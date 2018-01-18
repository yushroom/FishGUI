#pragma once

#include <FishGUI/ModelView/TreeWidget.hpp>
#include "FileNode.hpp"

#include <iostream>


class DirTreeModel : public FishGUI::TItemModel<FileNode*>
{
	typedef FileNode* T;
public:
	virtual T Parent(T child) const override
	{
		return child->parent;
	}
	
	inline FileNode* ChildAt(int index, FileNode* parent) const override
	{
		return parent->subdirs[index];
	}

	inline int ChildCount(FileNode* parent) const override
	{
		if (parent == nullptr)
			return 0;
		return static_cast<int>(parent->subdirs.size());
	}

	inline std::string Text(FileNode* item) const override
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
		m_selectionModel.SelectItem(m_root);
	}
};
