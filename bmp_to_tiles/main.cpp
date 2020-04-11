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
using std::multimap;
#include <vector>
using std::vector;
#include <algorithm>
#include <utility>
#include <iostream>
using std::ostringstream;
#include <iterator>
using std::ostream_iterator;

#include "bitmap.h"

template<typename A, typename B>
std::pair<B,A> FlipPair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> FlipMap(const std::map<A,B> &src)
{
    multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
                   FlipPair<A,B>);
    return dst;
}

string FormatTile(const string &in, const size_t tileWidth)
{
	vector<string> out;
    out.reserve(in.size() + in.size() / tileWidth);

	// split the string
    for(int i = 0; i < tileWidth; i++)
	{
        out.push_back(in.substr(i*tileWidth, tileWidth));
    }

	// reverse the string order
	std::reverse(out.begin(),out.end());

	// compile back to a string
	ostringstream os;
	std::copy(out.begin(), out.end()-1, 
		        ostream_iterator<string>(os, "\n")); 
	os << out.back();

    return os.str();
}

int main(int argc, char** argv)
{
	if(argc != 2 && argc != 3)
	{
		std::cerr << "Usage: " << argv[0] 
			<< " filename.bmp [palette.txt]" << std::endl;
		return 1;
	}

	Bitmap bmp;
	bmp.Read(argv[1]);

	char temp = 0;
	auto dataSize = ((bmp.infoHeader.width * 3 + 3) & (~3)) * bmp.infoHeader.height;

	cout << "Datasize: " << dataSize << endl;


	map<string, int> paletteIndex;
	if(argc == 3)
	{
		std::ifstream file(argv[2]);
		if(file.is_open())
		{
			string line;
			while(std::getline(file, line))
			{
				if(line.length() == 7);
				{
					paletteIndex[line.substr(2,5)] = std::stoi(line.substr(0,1));
				}
			}
		}
	}

	if(paletteIndex.size() == 0)
	{
		paletteIndex["$0000"] = 0;
	}
	int currentPaletteIndex = paletteIndex.size();

	
	int pixelCount = 0;
	int widthCounter = 0;
	int spriteRowCounter = 0;

	int numTiles = (bmp.infoHeader.width * bmp.infoHeader.height) / 64;

	vector<stringstream> tiles;
	tiles.reserve(numTiles);

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

	unsigned char rbSwapTmp;

	for(int i = 0; i < dataSize; i += 3)
	{
		// Swap R and B values:
		//if(bmp.data[i] != bmp.data[i+2])
		//{
		//	rbSwapTmp = bmp.data[i];
		//	bmp.data[i] = bmp.data[i+2];
		//	bmp.data[i+2] = rbSwapTmp;
		//}

		stringstream ssPalette;
		ssPalette << "$0" << std::uppercase << std::hex <<	
			(int(bmp.data[i+2] & 0xff) >> 4) <<
			(int(bmp.data[i+1] & 0xff) >> 4) <<
			(int(bmp.data[i] & 0xff) >> 4);

		string color = ssPalette.str();

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

	cout << endl;

	int numFinalTiles = tiles.size();
	int tilesByWidth = bmp.infoHeader.width / 8;
	int tilesByHeight = bmp.infoHeader.height / 8;

	vector<string> reorderedTiles;

	// Reorder list
	for(int i = 0; i < numFinalTiles; i += tilesByWidth)
	{
		for(int j = 0; j < tilesByWidth; j++)
		{
			reorderedTiles.push_back(tiles[numFinalTiles-i+j-tilesByWidth].str());
		}
	}

	// Format all tiles
	for(int i = 0; i < numFinalTiles; i++)
	{
		reorderedTiles[i] = FormatTile(reorderedTiles[i], size_t(8));;
	}


	cout << "Pixel Count: " <<  pixelCount << endl;
	cout << "Tile Count: " << tiles.size() << endl;

	cout << "stop...\n"; 

	cout << "palette\n";
	multimap<int, string> newMap = FlipMap(paletteIndex);
	for(auto const& palette : newMap)
	{
		cout << palette.first << "," << palette.second << endl;
	}

	int c = 1;   
	for (auto const& tile : reorderedTiles)
	{
		cout << c << ": \n";
		cout << tile << endl;
		c++;
	}

	return 0;
}
