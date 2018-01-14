#pragma once

#include "ItemModel.hpp"
//#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <FishGUI/Utils.hpp>

#include <iostream>

typedef boost::filesystem::path Path;
namespace fs = boost::filesystem;

struct FileNode
{
	Path path;
	std::string fileName;
	std::vector<FileNode*> subdirs;
	std::vector<FileNode*> files;
	
	FileNode(const Path& rootDir) : path(rootDir)
	{
		fileName = path.stem().string();
		if (!fs::is_directory(path))
		{
			return;
		}
		
		fs::directory_iterator end;
		for (fs::directory_iterator it(path); it != end; ++it)
		{
			auto p = it->path();
			auto fn = p.filename();
			if (fn.c_str()[0] == '.')		// hidden file
				continue;
			if (fn.extension() == ".meta")	// .meta file
				continue;
			if (fs::is_directory(p))
			{
				auto n = new FileNode(p);
				subdirs.push_back(n);
			}
			else
			{
				auto n = new FileNode(p);
				files.push_back(n);
			}
		}
	}
	
	FileNode* Find(const Path& p)
	{
		auto r = fs::relative(p, path);
		std::cout << r << std::endl;
		std::cout << r.root_directory() << std::endl;
		return nullptr;
	}
};


typedef TListModel<FileNode, std::string> FileListModel;

template<>
inline FileNode* FileListModel::childAt(FileNode* parent, int row) const
{
	return parent->files[row];
}

template<>
inline int FileListModel::rowCount(FileNode* parent) const
{
//	if (parent == m_root)
	return parent->files.size();
//	return 0;
}

template<>
inline std::string	FileListModel::data(FileNode* item) const
{
	return std::string(item->fileName);
}



typedef TTreeModel<FileNode, std::string> DirTreeModel;

template<>
inline FileNode* DirTreeModel::childAt(FileNode* parent, int row) const
{
	return parent->subdirs[row];
}

template<>
inline int DirTreeModel::rowCount(FileNode* parent) const
{
	if (parent == nullptr)
		return 0;
	return parent->subdirs.size();
}

template<>
inline std::string DirTreeModel::data(FileNode* item) const
{
	return item->fileName;
}

template<>
inline const FontIcon*	DirTreeModel::icon(FileNode* item) const
{
	static char icon[8];
	static FontIcon folderIcon = {CodePointToUTF8(0xe930, icon), 12, "ui"};
	return &folderIcon;
}
	
//	virtual const FontIcon* icon(FileNode* item) const override
//	{
////		if (IsItemFolded(item))
////		{
////			return &icon2;
////		}
////		return &icon1;
//		return &iconFolder;
//	}

