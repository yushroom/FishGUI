#pragma once

#include <FishGUI/Widget.hpp>
#include "FileItemModel.hpp"
#include <FishGUI/ModelView/ItemView.hpp>


class FileListViewModel : public FishGUI::TListModel<FileNode*>
{
public:
	typedef FileNode* T;

	virtual int count(T item) const override
	{
		//assert(item == m_root);
		return static_cast<int>( item->files.size() );
	}

	virtual T childAt(int row, int column, T parent) const override
	{
		//assert(parent == m_root);
		int idx = row * m_columns + column;
		if (idx >= 0 && idx < parent->files.size())
			return parent->files[idx];
		return nullptr;
	}

	virtual std::string text(T item) const override
	{
		return item->fileName;
	}
};

class FileListWidget : public FishGUI::TItemView<FileNode*>
{
public:
	typedef TItemView<FileNode*> Super;
	FileListWidget(const char* name);

	virtual void Render() override;

	void SetRoot(FileNode* root)
	{
		m_root = root;

		//auto model = dynamic_cast<TListModel<FileNode*>>
		//m_model->SetRoot(root);
		m_imContext->Reset();

		//m_selectionModel.selectItem(m_model->childAtIndex(0, m_root));
	}

protected:
	FileNode* m_root;
};
