#include <stdio.h>
#include <vector>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"
#include "Bitmap.hpp"

unsigned char buffer[24<<20];
unsigned char screen[20][79];

int main(int arg, char **argv)
{
   stbtt_fontinfo font;
   int i,j,ascent,baseline,ch=0;
   float scale, xpos=2; // leave a little padding in case the character extends left
   const char *text = "Heljo World!"; // intentionally misspelled to show 'lj' brokenness

   //fread(buffer, 1, 1000000, fopen("/System/Library/Fonts/Menlo.ttc", "rb"));
   fread(buffer, 1, 1000000, fopen("MSYH.TTF", "rb"));
   stbtt_InitFont(&font, buffer, 0);

   scale = stbtt_ScaleForPixelHeight(&font, 15);
   stbtt_GetFontVMetrics(&font, &ascent,0,0);
   baseline = (int) (ascent*scale);

   while (text[ch]) {
      int advance,lsb,x0,y0,x1,y1;
      float x_shift = xpos - (float) floor(xpos);
      stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);
      stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale,scale,x_shift,0, &x0,&y0,&x1,&y1);
      stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline + y0][(int) xpos + x0], x1-x0,y1-y0, 79, scale,scale,x_shift,0, text[ch]);
      // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
      // because this API is really for baking character bitmaps into textures. if you want to render
      // a sequence of characters, you really need to render each bitmap to a temp buffer, then
      // "alpha blend" that into the working buffer
      xpos += (advance * scale);
      if (text[ch+1])
         xpos += scale*stbtt_GetCodepointKernAdvance(&font, text[ch],text[ch+1]);
      ++ch;
   }

   std::vector<uint8_t> pixels;
   pixels.reserve(20*78*3);
   for (j=0; j < 20; ++j) {
      for (i=0; i < 78; ++i) {
         //putchar(" .:ioVM@"[screen[j][i]>>5]);
         unsigned char p = screen[j][i];
         for (int k = 0; k < 3; ++k)
            pixels.push_back(255-p);
      }
      //putchar('\n');
   }
   write_bmp("./fonts2.bmp", 78, 20, 3, pixels);
   return 0;
}
