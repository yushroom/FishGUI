#include <stdio.h>
#include <vector>
#include <stdint.h>
#include "Bitmap.hpp"
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

unsigned char ttf_buffer[1<<25];

const int WIDTH = 512;
const int HEIGHT = 512;
unsigned char screen[512][512];

bool is_edge(int x, int y) {
   if (x==0 || y==0 || x==WIDTH-1 || y==HEIGHT-1)
      return false;
   return screen[y][x] != 0 && 
   (screen[y-1][x-1]==0 || screen[y-1][x]==0 || screen[y-1][x+1]==0 || screen[y][x-1]==0 ||
      screen[y][x+1]==0 || screen[y+1][x-1]==0 || screen[y+1][x]==0 || screen[y+1][x+1]==0);
}

int main(int argc, char **argv)
{
   stbtt_fontinfo font;
   unsigned char *bitmap;
   int w,h,i,j,c = (argc > 1 ? atoi(argv[1]) : 'g'), s = (argc > 2 ? atoi(argv[2]) : 512);

   //fread(ttf_buffer, 1, 1<<25, fopen(argc > 3 ? argv[3] : "c:/windows/fonts/arialbd.ttf", "rb"));
   fread(ttf_buffer, 1, 1<<25, fopen(argc > 3 ? argv[3] : "MSYH.TTF", "rb"));

   stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
   // int ascent = 0; 
   // int descent = 0;
   // int lineGap = 0;
   // stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
   // int mLineHeight = ascent - descent + lineGap;
   // printf("line height: %d\n", mLineHeight);

   int ascent, baseline;
   float scale = stbtt_ScaleForPixelHeight(&font, s);
   stbtt_GetFontVMetrics(&font, &ascent, 0, 0);
   baseline = (int)(ascent*scale);
   printf("baseline = %d\n", baseline);

   int x0, y0, x1, y1;

   memset(screen, 0, s*s);
   float x_shift = 0;
   stbtt_GetCodepointBitmapBoxSubpixel(&font, c, scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
   printf("%d %d %d %d\n", x0, y0, x1, y1);
   int width = x1 - x0;
   float offset = (s-x1-x0)/2.0f;
   printf("%lf\n", offset);
   x_shift = offset - floor(offset);
   stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline+y0][(int)(x0+offset)], x1-x0, y1-y0, s, scale, scale, x_shift, 0, c);

   std::vector<uint8_t> pixels;
   w = h = s;
   pixels.reserve(w*h*3);
   for (j=0; j < h; ++j) {
      for (i=0; i < w; ++i) {
         //putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
         //auto p = screen[j][i];
         uint8_t p = is_edge(i, j) ? 255 : 0;
         for (int k = 0; k < 3; ++k)
            pixels.push_back(p);
      }
      //putchar('\n');
   }
   write_bmp("./fonts2.bmp", w, h, 3, pixels);
   return 0;
}