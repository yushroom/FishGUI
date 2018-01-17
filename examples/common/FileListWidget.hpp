#pragma once

#include "FileNode.hpp"
#include <FishGUI/ModelView/ListWidget.hpp>


class FileListModel : public FishGUI::TItemModel<FileNode*>
{
	typedef FileNode* T;
public:
	virtual T parent(T child) const override
	{
		return child->parent;
	}

	virtual int childCount(T item) const override
	{
		//assert(item == m_root);
		return static_cast<int>( item->files.size() );
	}

	virtual T childAt(int index, T parent) const override
	{
		//assert(parent == m_root);
		return parent->files[index];
		//return nullptr;
	}

	virtual std::string text(T item) const override
	{
		return item->fileName;
	}
};

class FileListWidget : public FishGUI::ListWidget<FileNode*>
{
	typedef ListWidget<FileNode*> Super;
public:
	FileListWidget(const char* name);
};
