#pragma once

#include "FileNode.hpp"
#include <FishGUI/ModelView/ListWidget.hpp>


class FileListModel : public FishGUI::TListModel<FileNode*>
{
public:
	typedef FileNode* T;

	virtual int childCount(T item) const override
	{
		//assert(item == m_root);
		return static_cast<int>( item->files.size() );
	}

	virtual T childAt(int row, int column, T parent) const override
	{
		//assert(parent == m_root);
		//int idx = row * m_columns + column;
		//if (idx >= 0 && idx < parent->files.size())
			return parent->files[row];
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
