#pragma once

#include <string>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

// from https://stackoverflow.com/questions/516200/relative-paths-not-working-in-xcode-c
// ----------------------------------------------------------------------------
// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
#ifdef __APPLE__
inline std::string ApplicationFilePath()
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
//#include <WinBase.h>

//// Convert a wide Unicode string to an UTF8 string
//inline std::string utf8_encode(const std::wstring &wstr)
//{
//	if (wstr.empty()) return std::string();
//	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
//	std::string strTo(size_needed, 0);
//	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
//	return strTo;
//}
//
//// Convert an UTF8 string to a wide Unicode String
//inline std::wstring utf8_decode(const std::string &str)
//{
//	if (str.empty()) return std::wstring();
//	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
//	std::wstring wstrTo(size_needed, 0);
//	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
//	return wstrTo;
//}

inline std::string ApplicationFilePath()
{
	auto hModule = GetModuleHandleW(NULL);
	//WCHAR path[MAX_PATH];
	//GetModuleFileNameW(hModule, path, MAX_PATH);
	//std::wstring ws(path);
	char path2[MAX_PATH];
	GetModuleFileNameA(hModule, path2, MAX_PATH);
	std::experimental::filesystem::path p(path2);
	return p.parent_path().string();
	//return std::experimental::filesystem::current_path().string();
	//WideCharToMultiByte()
	//return std::string(path2);
}
#endif

inline char* CodePointToUTF8(int cp, char* str)
{
	int n = 0;
	if (cp < 0x80) n = 1;
	else if (cp < 0x800) n = 2;
	else if (cp < 0x10000) n = 3;
	else if (cp < 0x200000) n = 4;
	else if (cp < 0x4000000) n = 5;
	else if (cp <= 0x7fffffff) n = 6;
	str[n] = '\0';
	switch (n) {
		case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
		case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
		case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
		case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
		case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
		case 1: str[0] = cp;
	}
	return str;
}
