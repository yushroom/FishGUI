#pragma once

#include <string>

struct FontIcon
{
//	int codePoint;	// code point in .ttf
	std::string fontText;	// utf8
	int 		fontSize;
	const char* fontFace;
};