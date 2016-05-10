#ifndef RFFont_hpp
#define RFFont_hpp

#define _CRT_SECURE_NO_WARNINGS

#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>
#include "Bitmap.hpp"

//#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

class RFFont {
public:
	RFFont(const char* font_path);

	~RFFont() {
	}

	static std::shared_ptr<RFFont> GetInstance()
    {
        //static auto instance = std::make_shared<RFFont>("/Users/yushroom/program/graphics/RFGUI/src/MSYH.TTF");
		static auto instance = std::make_shared<RFFont>("c:/windows/fonts/MSYH.TTF");
        return instance;
	}

//	std::shared_ptr<Bitmap> GetBitmapForChar(const char c, const int size)
//    {
//        
//	}
//
    std::shared_ptr<Bitmap> GetBitmapForString(const std::string& text, const int w, const int h);
private:
    unsigned char m_buffer[24<<20];
    stbtt_fontinfo m_font;
};


#endif // RFFont_hpp