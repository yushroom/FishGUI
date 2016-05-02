#ifndef Bitmap_hpp
#define Bitmap_hpp


#include <string>
#include <vector>
#include <fstream>
#include <cmath>
using namespace std;

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;

#pragma pack(2)
struct BitmapFileHeader {
    WORD	type;			//== 0x424D "BM"
    DWORD	size;
    WORD	reserved1;
    WORD	reserved2;
    DWORD	offBits;
};	// 14 bytes

struct BitmapInfoHeader {
    DWORD	size;		// 40, size of this struct
    LONG	width;		// pixels
    LONG	height;		// pixels
    WORD	planes;		// 1
    WORD	bitcount;	// 1 for binary, 4 for 16bit, 8 for 256bit, 24 for true color
    DWORD	compression;
    DWORD	sizeImage;
    LONG	XPelsPerMeter;
    LONG	YPelsPerMeter;
    DWORD	clrUsed;
    DWORD	clrImportant;
};	// 40 bytes

inline float saturate(const float x) {
    return x < 0.f ? 0.f : x > 1.f ? 1.f : x;
}

inline BYTE tobyte(float x) {
    return BYTE(powf(saturate(x), 1 / 2.2f) * 255 + .5f);;
}

inline void write_bytes(ostream& fout, WORD x) {
    fout << BYTE(x) << BYTE(x>>8);
}

inline void write_bytes(ostream& fout, DWORD x) {
    fout << BYTE(x) << BYTE(x>>8) << BYTE(x>>16) << BYTE(x>>24);
}

void write_bmp(
               const string& path,
               const size_t width,
               const size_t height,
               const vector<uint8_t>& pixels) {
    
    //auto s1 = sizeof(BitmapFileHeader);
    //auto s2 = sizeof(BitmapInfoHeader);
    //assert(sizeof(BitmapFileHeader) == 14);
    assert(sizeof(BitmapInfoHeader) == 40);
    assert(pixels.size() == width * height * 3);
    const size_t bytes = width * height * 3;
    ofstream fout(path, std::ios::binary);
    
    BitmapFileHeader header;
    header.type = 0x4D42;
    header.size = DWORD(54 + bytes);
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.offBits = 54;
    //fout.write((const char*)&header, 14);
    //fout << header.type << header.size << header.reserved1 << header.reserved1 << header.offBits;
    write_bytes(fout, header.type);
    write_bytes(fout, header.size);
    write_bytes(fout, header.reserved1);
    write_bytes(fout, header.reserved2);
    write_bytes(fout, header.offBits);
    
    BitmapInfoHeader info_header;
    memset(&info_header, 0, sizeof(BitmapInfoHeader));
    info_header.size = 40;
    info_header.height = (LONG)height;
    //info_header.height = -info_header.height;
    info_header.width = (LONG)width;
    info_header.planes = 1;
    info_header.bitcount = 24;
    info_header.sizeImage = (DWORD)bytes;
    fout.write((const char*)&info_header, 40);
    
    //    vector<BYTE> p(bytes);
    //    int i = 0;
    //    for (int y = (int)height - 1; y >= 0; --y)
    //        for (int x = 0; x < (int)width; ++x) {
    //            auto& c = pixels[y*width+x];
    //            p[i++] = tobyte(c.z);
    //            p[i++] = tobyte(c.y);
    //            p[i++] = tobyte(c.x);
    //            //fout << tobyte(c.z) << tobyte(c.y) << tobyte(c.x);
    //        }
    fout.write((const char*)&pixels[0], bytes);
    fout.close();
}

#endif