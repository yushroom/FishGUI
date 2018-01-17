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
