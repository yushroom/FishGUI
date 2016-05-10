#define _CRT_SECURE_NO_WARNINGS
#include "RFFont.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <vector>

using namespace std;

RFFont::RFFont(const char* font_path) {
	FILE* fp = fopen(font_path, "rb");
	//        if (!fp) {
	//            std::perror("File opening failed");
	//        }
	assert(fp);
	fread(m_buffer, 1, 10000000, fp);
	stbtt_InitFont(&m_font, m_buffer, 0);
}

std::shared_ptr<Bitmap> RFFont::GetBitmapForString(const std::string& text, const int w, const int h) {
    float scale = stbtt_ScaleForPixelHeight(&m_font, h);
    int ascent, baseline;
    stbtt_GetFontVMetrics(&m_font, &ascent, 0, 0);
    baseline = (int)(ascent*scale);
    float xpos = 0; // leave a little padding in case the character extends left
    int width = 0;
    //int height = h;
    
    auto bmp = make_shared<Bitmap>(w, h, 1);
    
    //for (char ch : text) {
    for (int i = 0; i < text.size(); ++i) {
        char ch = text[i];
        int advance, lsb, x0, y0, x1, y1;
        float x_shift = xpos - (float) floor(xpos);
        stbtt_GetCodepointHMetrics(&m_font, ch, &advance, &lsb);
        stbtt_GetCodepointBitmapBoxSubpixel(&m_font, ch, scale,scale,x_shift,0, &x0,&y0,&x1,&y1);
        
        int idx = (baseline+y0)*w + (int)xpos + x0;
        stbtt_MakeCodepointBitmapSubpixel(&m_font, &bmp->pixels[idx], x1-x0,y1-y0, w, scale,scale,x_shift,0, ch);
        xpos += (advance * scale);
        if (i < text.size()-1)
            xpos += scale*stbtt_GetCodepointKernAdvance(&m_font, ch,text[i+1]);
        width = (int)xpos + x0;
    }
    //printf("width = %d\n", width);
    return bmp;
}
