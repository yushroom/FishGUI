#ifndef RFFont_hpp
#define RFFont_hpp

#include <cassert>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Bitmap.hpp"

class RFFont {
public:
	RFFont(const char* font_path) {
		FT_Error error;
		error = FT_Init_FreeType(&library);              /* initialize library */
		assert(!error);
		error = FT_New_Face(library, font_path, 0, &face);/* create face object */
		assert(!error);
	}

	~RFFont() {
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}

	static std::shared_ptr<RFFont> get_instance() {
		static auto instance = std::make_shared<RFFont>(R"(C:\Windows\Fonts\consola.ttf)");
		return instance;
	}

	std::shared_ptr<Bitmap> get_bitmap_for_char(const char c, const int size) {
		FT_Error error;
		error = FT_Set_Char_Size(face, size << 6, 0, 96, 0);                /* set character size */
		error = FT_Set_Pixel_Sizes(face, size, 0);
		assert(!error);
		error = FT_Load_Char(face, c, FT_LOAD_RENDER);
		assert(!error);

		auto slot = face->glyph;
		auto& fbmp = slot->bitmap;
		int width = fbmp.width;
		int height = fbmp.rows;
		auto bmp = std::make_shared<Bitmap>(width, height);
		for (int j = 0; j < height; ++j) {
			for (int i = 0; i < width; ++i) {
				int idx = j * width + i;
				//uint8_t p = fbmp.buffer[idx] == 0 ? 0 : 255;
				auto& p = fbmp.buffer[idx];
				bmp->pixels[idx * 3] = p;
				bmp->pixels[idx * 3 + 1] = p;
				bmp->pixels[idx * 3 + 2] = p;
			}
		}
		return bmp;
	}

private:
	FT_Library    library;
	FT_Face       face;
};


#endif // RFFont_hpp