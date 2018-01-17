#pragma once

#include <iostream>
#include <FishGUI/Utils.hpp>

#ifdef _WIN32
#include <filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;
#endif
typedef fs::path Path;


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
	
	// FileNode* Find(const Path& p)
	// {
	// 	auto r = fs::relative(p, path);
	// 	std::cout << r << std::endl;
	// 	std::cout << r.root_directory() << std::endl;
	// 	return nullptr;
	// }
};
