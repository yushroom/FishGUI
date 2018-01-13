#pragma once

#include <FishGUI/Widget.hpp>
#include "../common/FileItemModel.hpp"
#include "../common/TSelectionModel.hpp"

class FileListWidget : public FishGUI::IMWidget
{
public:
	FileListWidget(const char* name);
	virtual void DrawImpl() override;
	
	void SetRoot(FileNode* root)
	{
		m_root = root;
		m_imContext->Reset();
	}
	
private:
	FileListModel m_model;
	TSelectionModel<FileNode> m_selectionModel;
	FileNode* m_root;
};
