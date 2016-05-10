#define _CRT_SECURE_NO_WARNINGS
#include "Bitmap.hpp"

#include <cassert>
#include <fstream>
#include <cmath>
#include <algorithm>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
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
	fout << BYTE(x) << BYTE(x >> 8);
}

inline void write_bytes(ostream& fout, DWORD x) {
	fout << BYTE(x) << BYTE(x >> 8) << BYTE(x >> 16) << BYTE(x >> 24);
}

void write_bmp(
	const string& path,
	const size_t width,
	const size_t height,
	const vector<uint8_t>& pixels) {

	//auto s1 = sizeof(BitmapFileHeader);
	//auto s2 = sizeof(BitmapInfoHeader);
	assert(sizeof(BitmapFileHeader) == 14);
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

	//const LONG width_with_padding = (LONG)ceilf(width / 4.0f) * 4;
	const int row_length = width * 3;
	const int row_length_padding = (LONG)ceil(row_length/4.0f) * 4;
	bool has_padding = (row_length != row_length_padding);
	info_header.sizeImage = DWORD(row_length_padding * height);
	fout.write((const char*)&info_header, 40);

	if (!has_padding)
		fout.write((const char*)&pixels[0], bytes);
	else {
		vector<uint8_t> p(row_length_padding * height, 0);
		for (int i = 0; i < height; ++i) {
			const int start1 = row_length*i;
			const int start2 = row_length_padding*i;
			std::copy(pixels.begin()+start1, pixels.begin()+start1+row_length, p.begin()+start2);
		}
		fout.write((const char*)&p[0], row_length_padding * height);
	}
	fout.close();
}

void write_bmp(
               const std::string& path,
               const size_t width,
               const size_t height,
               const int channels,
               const std::vector<uint8_t>& pixels)
{
    stbi_write_bmp(path.c_str(), width, height, channels, (void*)&pixels[0]);
}

void write_bmp(
	const std::string& path,
	const size_t width,
	const size_t height,
	const int channels,
	uint8_t* pixels)
{
	stbi_write_bmp(path.c_str(), width, height, channels, (void*)pixels);
}

// void write_png(
// 	const std::string& path,
// 	const size_t width,
// 	const size_t height,
// 	const int channels,
// 	uint8_t* pixels)
// {
// 	stbi_write_png
// }

void Bitmap::save(const std::string& path) const {
	//write_bmp(path, width, height, pixels);
    
    if (m_channels == 3)
        stbi_write_bmp(path.c_str(), width, height, m_channels, (void*)pixels.data());
    else if (m_channels == 1){
        vector<uint8_t> p;
        p.reserve(width * height * 3);
        for (auto pp : pixels) {
            for (int i = 0; i < 3; ++i)
                p.push_back(pp);
        }
        stbi_write_bmp(path.c_str(), width, height, 3, (void*)p.data());
    } else {
        assert(false);
    }
}