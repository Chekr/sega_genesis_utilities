#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <unordered_set>
using std::unordered_set;
#include <sstream>
using std::stringstream;

#include <map>
using std::map;
#include <vector>
using std::vector;


#include "bitmap.h"



int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cerr << "Usage: " << argv[0] 
			<< " filename.bmp" << std::endl;
		return 1;
	}

	Bitmap bmp;
	bmp.Read(argv[1]);

	char temp = 0;
	auto dataSize = ((bmp.infoHeader.width * 3 + 3) & (~3)) * bmp.infoHeader.height;

	cout << "Datasize: " << dataSize << endl;

	std::unordered_set<string> palette;

	//DC.W    $0000,$0044,$0066,$0088

	map<string, int> paletteIndex;
	int currentPaletteIndex = 0;

	vector<stringstream> tiles;
	
	int pixelCount = 0;

	int widthCounter = 0;
	int spriteRowCounter = 0;

	int numTiles = (bmp.infoHeader.width * bmp.infoHeader.height) / 64;
	cout << "Width: " << bmp.infoHeader.width << endl;
	cout << "Height: " << bmp.infoHeader.height << endl;
	cout << "Num Tiles: " << numTiles << endl;

	for(int i = 0; i < numTiles; i++)
	{
		tiles.push_back(stringstream());
		tiles[i] << std::uppercase << std::hex;
	}
	cout << "Tile mem:" << tiles.size() << endl;
	

	cout << "start...\n";

	for (auto i = 0; i < dataSize; i += 3)
	{
		temp = bmp.data[i];
		bmp.data[i] =  bmp.data[i+2];
		bmp.data[i+2] = temp;

		stringstream ssPalette;
		ssPalette << "$0" << std::uppercase << std::hex <<	
			(int(bmp.data[i+2] & 0xff) >> 4) <<
			(int(bmp.data[i+1] & 0xff) >> 4) <<
			(int(bmp.data[i] & 0xff) >> 4);

		string color = ssPalette.str();

		palette.insert(color);

		if(paletteIndex.find(color) == paletteIndex.end() )
		{
			paletteIndex[color] = currentPaletteIndex;
			currentPaletteIndex++;
		}

		int pIndex = paletteIndex[color];
		int tIndex = ((spriteRowCounter/8) * (bmp.infoHeader.width / 8)) + (widthCounter / 8);
		tiles[tIndex] << pIndex;

		if(widthCounter == bmp.infoHeader.width - 1)
		{
			widthCounter = 0;
			spriteRowCounter++;
		}
		else
		{
			widthCounter++;
		}
		pixelCount++;
	}
	cout << "Pixel Count: " <<  pixelCount << endl;

	//for(auto color : palette)
	//{
	//	cout << color << endl;
	//}
	cout << "stop...\n"; 

	cout << "palette\n";
	for(auto const& palette : paletteIndex)
	{
		cout << palette.first << "," << palette.second << endl;
	}

	cout << "tiles\n";
	int c = 0;
	for (auto const& tile : tiles)
	{
		cout << c << ": " << tile.str() << endl;
		c++;
	}

	return 0;
}
