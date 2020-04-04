#ifndef _BITMAP_H_
#pragma once
#define _BITMAP_H_

#include <fstream>
#include <vector>
using std::vector;

// https://en.wikipedia.org/wiki/BMP_file_format

#pragma pack(push, 1)
struct BitmapFileHeader
{
public:
	uint16_t fileType = 0; // 0x4D42 (bitmap)
	uint32_t fileSize = 0;
	uint16_t reserved1 = 0;
	uint16_t reserved2 = 0;
	uint32_t dataOffset = 0;
};

// DIB Header, BITMAPCOREHEADER2 has an extra 24 bytes over this
struct WindowsBitmapInfoHeader // BITMAPINFOHEADER
{
public:
	uint32_t size = 0;
	int32_t width = 0;
	int32_t height = 0;

	uint16_t planes = 1; // must be 1
	uint16_t bitsPerPixel = 0; // 1, 4, 8, 16, 24, 32
	uint32_t compressionMethod = 0;
	uint32_t imageSize = 0;
	int32_t horizontalResolition = 0; // pixel per meter
	int32_t verticalResolution = 0; // pixel per meter
	uint32_t colorsInPalette = 0; // 0 to 2^n
	uint32_t numberImportantColorsUsed = 0; // 0 when all are important
};

#pragma pack(pop)


class Bitmap
{
public:
	const uint16_t bitmapIdentifier = 0x4D42;
	BitmapFileHeader fileHeader;
	WindowsBitmapInfoHeader infoHeader;

	vector<uint8_t> data;

public:
	void Read(const char* filename)
	{
		std::ifstream bmp(filename, std::ios::binary);
		if(bmp)
		{
			// Get file header
			bmp.read((char*)&fileHeader, sizeof(fileHeader));
			
			if(!bmp)
			{
				throw std::runtime_error("Cannot load file.");
			}

			if(fileHeader.fileType != bitmapIdentifier)
			{
				throw std::runtime_error("Could not find proper file header.");
			}

			// Get bitmap header
			bmp.read((char*)&infoHeader, sizeof(infoHeader));

			/*	
			cout << 
			infoHeader.size << "|" <<
			infoHeader.width << "|" <<
			infoHeader.height << "|" <<
			infoHeader.planes << "|" <<
			infoHeader.bitsPerPixel << "|" <<
			infoHeader.compressionMethod << "|" <<
			infoHeader.imageSize << "|" <<
			infoHeader.horizontalResolition << "|" <<
			infoHeader.verticalResolution << "|" <<
			infoHeader.colorsInPalette << "|" <<
			infoHeader.numberImportantColorsUsed << endl;
			*/

			if(infoHeader.width <= 0 || infoHeader.height <= 0)
			{
				throw std::runtime_error("Refusal to handle negative sizes");
			}

			if(infoHeader.width % 4 != 0 || infoHeader.height % 4 != 0)
			{
				throw std::runtime_error("size needs to be divisible by 4");
			}

			if(infoHeader.bitsPerPixel == 32)
			{
				// TODO: implement this
				throw std::runtime_error("Cannot support 32-bit bitmnaps (transparency layer included)");
			}

			// Go to pixel data
			bmp.seekg(fileHeader.dataOffset, bmp.beg);

			// Should be updated for 32-bit...
			infoHeader.size = sizeof(WindowsBitmapInfoHeader);
			fileHeader.dataOffset = sizeof(fileHeader) + sizeof(WindowsBitmapInfoHeader);
			fileHeader.fileSize = fileHeader.dataOffset;

			data.resize(infoHeader.width*infoHeader.height*infoHeader.bitsPerPixel / 8);

			// Should handle padding data at some point... (make divisible by 4)	
			bmp.read((char*)data.data(), data.size());
			fileHeader.fileSize += data.size();
		}
		else
		{
			throw std::runtime_error("Cannot open image");
		}
	}
};

#endif
