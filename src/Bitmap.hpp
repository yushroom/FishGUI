#ifndef Bitmap_hpp
#define Bitmap_hpp

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

class Bitmap {
public:
	Bitmap(const int width, const int height) 
		: width(width), height(height), pixels(width*height*3, 0) {}

	void save(const std::string& path) const;

	const int width;
	const int height;
	std::vector<uint8_t> pixels;
};

void write_bmp(
	const std::string& path,
	const size_t width,
	const size_t height,
	const std::vector<uint8_t>& pixels);

#endif