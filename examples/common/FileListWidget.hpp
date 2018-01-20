#pragma once

#include "FileNode.hpp"
#include <FishGUI/ModelView/ListWidget.hpp>
#include <FishGUI/Utils.hpp>


class FileListModel : public FishGUI::TItemModel<FileNode*>
{
	typedef FileNode* T;
public:
	virtual T Parent(T child) const override
	{
		return child->parent;
	}

	virtual int ChildCount(T item) const override
	{
		//assert(item == m_root);
		return static_cast<int>( item->subdirs.size() + item->files.size());
	}

	virtual T ChildAt(int index, T parent) const override
	{
		//assert(parent == m_root);
		if (index < parent->subdirs.size())
			return parent->subdirs[index];
		return parent->files[index - parent->subdirs.size()];
	}

	virtual std::string Text(T item) const override
	{
		return item->fileName;
	}

	virtual const FishGUI::Icon* Icon(T item) const override
	{
		constexpr int dirIconCP = 0xf115;
		constexpr int fileIconCP = 0xf016;
		char icon[8];
		static FishGUI::FontIcon dirIcon{ CodePointToUTF8(dirIconCP, icon), 64.f, "ui" };
		static FishGUI::FontIcon fileIcon{ CodePointToUTF8(fileIconCP, icon), 64.f, "ui" };
		return item->isDir ? &dirIcon : &fileIcon;
	}
};

class FileListWidget : public FishGUI::ListWidget<FileNode*>
{
	typedef ListWidget<FileNode*> Super;
public:
	FileListWidget(const char* name) : Super(name)
	{
		m_model = new FileListModel();
		m_selectionModel.SetSelectionChangedCallback([](FileNode* selected) {
			if (selected != nullptr)
				std::cout << selected->path << std::endl;
		});
	}
};


class UnityFileWidget : public FishGUI::Widget
{
public:
	UnityFileWidget(const char* name) : Widget(name), m_listWidget("")
	{
		m_listWidget.GetSelectionModel()->SetSelectionChangedCallback([this](FileNode* selected) {
			m_selectedFile = selected;
		});
	}

	virtual void Draw() override
	{
		constexpr int bottomLabelHeight = 20;
		auto r = m_rect;
		m_listWidget.SetRect(r.x, r.y, r.width, r.height - bottomLabelHeight);
		m_listWidget.BindAndDraw();
		std::string filename = "";
		if (m_selectedFile != nullptr)
			filename = Path(m_selectedFile->path).filename().string();
		r.y = r.bottom() - bottomLabelHeight;
		r.height = bottomLabelHeight;
		FishGUI::DrawRect(FishGUI::Context::GetInstance().m_drawContext, r, nvgRGB(221, 221, 221));
		FishGUI::Label(filename, r);
	}

	FileListWidget * GetFileListWidget() { return &m_listWidget; }

protected:
	FileListWidget m_listWidget;
	FileNode* m_selectedFile = nullptr;
};