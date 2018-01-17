#include <FishGUI/Utils.hpp>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"

// from https://stackoverflow.com/questions/516200/relative-paths-not-working-in-xcode-c
// ----------------------------------------------------------------------------
// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
std::string ApplicationFilePath()
{
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
	{
		// error!
		abort();
	}
	CFRelease(resourcesURL);

	chdir(path);
//	std::cout << "Current Path: " << path << std::endl;
	return std::string(path);
}
#endif
// ----------------------------------------------------------------------------

#if _WIN32
#include <filesystem>
#include <Shlwapi.h>

std::string ApplicationFilePath()
{
	auto hModule = GetModuleHandleW(NULL);
	char path2[MAX_PATH];
	GetModuleFileNameA(hModule, path2, MAX_PATH);
	std::experimental::filesystem::path p(path2);
	return p.parent_path().string();
}

#endif

#if __linux__
// #include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>	// memset

#include <boost/filesystem/path.hpp>

std::string ApplicationFilePath()
{
	// char buf[PATH_MAX];
	// char* cwd = getcwd(buf, PATH_MAX);
	// if (cwd == nullptr)
	// 	abort();
	// printf("cwd: %s\n", cwd);
	// return std::string(cwd);

	char dest[PATH_MAX];
	memset(dest, 0, sizeof(dest));
	if (readlink("/proc/self/exe", dest, PATH_MAX) == -1)
	{
		puts("[fatal error] readlink failed!");
		abort();
	}
	return boost::filesystem::path(dest).parent_path().string();
}

#endif
