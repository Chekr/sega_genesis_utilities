#include <iostream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <iterator>


#include <iomanip>

#include "../shared/img/bitmap.h"
#include "../shared/img/sega_img.h"


template<typename A, typename B>
std::pair<B,A> FlipPair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> FlipMap(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
                   FlipPair<A,B>);
    return dst;
}

std::string FormatTile(const std::string &in, const size_t tileWidth)
{
    std::vector<std::string> out;
    out.reserve(in.size() + in.size() / tileWidth);

	// split the string
    for(int i = 0; i < tileWidth; i++)
	{
        out.push_back(in.substr(i*tileWidth, tileWidth));
    }

	// reverse the string order
	std::reverse(out.begin(),out.end());

	// compile back to a string
    std::ostringstream os;
	std::copy(out.begin(), out.end()-1, 
		        std::ostream_iterator<std::string>(os, "\n")); 
	os << out.back();

    return os.str();
}



std::string ConvertPackedTileToBitplane(std::string s)
{
    std::vector<std::string> lines;
    std::stringstream ss(s);
    std::string token;
    // Separate each row
    while(std::getline(ss,token, '\n'))
    {
        lines.push_back(token);
    }
    
    std::vector<std::string> result;

    // For each row
    for(auto line : lines)
    {
        std::vector<std::string> binString;
        for(int i = 0; i < 8; i++)
        {
            // Char to hex
            std::stringstream ss2;
            ss2 << std::hex << line[i];
            unsigned n;
            ss2 >> n;
            std::bitset<4> b(n);
            binString.push_back(b.to_string());
        }

        // Each binary column to hex
        std::vector<std::string> binColumns;
        for(int i = 0; i < 4; i++)
        {
            std::stringstream ss2;
            for(int j = 0; j < 8; j++)
            {
                ss2 << binString[j][i];
            }
            binColumns.push_back(ss2.str());
        }

        // Convert to hex 
        std::vector<std::string> hexRow;
        for(int i = 0; i < 4; i++)
        {
            std::stringstream ss2;
            std::bitset<8> set(binColumns[i]);
            ss2 << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << set.to_ulong();
            hexRow.push_back(ss2.str());

        }     

        // make into a single row
        std::stringstream ss3;
        for(auto r : hexRow)
        {
            ss3 << r;
        }
        result.push_back(ss3.str());
    }

    std::stringstream finalResult;
    for(auto q : result)
    {
        finalResult << q << "\n";
    }


    std::string blah = finalResult.str();
    std::cout << blah.size() << " : " << blah << "\n";





    return finalResult.str();
};


int main(int argc, char** argv)
{
    /*
	if(argc != 2 && argc != 3)
	{
		std::cerr << "Usage: " << argv[0] 
			<< " filename.bmp [palette.txt]\n";
		return 1;
	}
    */

    int opt;
    PaletteType type;
	Bitmap bmp;
	//bmp.Read(argv[1]);
    std::map<std::string, int> paletteIndex;

    while((opt = getopt(argc, argv, "f:t:p:")) != -1)
    {
        switch(opt)
        {
            case 'f':
                bmp.Read(optarg);
                break;
            case 't':
                if(strcmp(optarg,"MEGA_DRIVE") == 0)
                {
                    type = MEGA_DRIVE;
                }
                else if(strcmp(optarg,"MASTER_SYSTEM") == 0)
                {
                    type = MASTER_SYSTEM;
                }
                else if(strcmp(optarg,"GAMEGEAR") == 0)
                {
                    type = GAMEGEAR;
                }
                else if(strcmp(optarg,"TMS") == 0)
                {
                    type = TMS;
                }
                else
                {
                    type = RGB;
                }
                break;
            case 'p':
                std::ifstream file(optarg);
                if(file.is_open())
                {
                    std::string line;
                    while(std::getline(file, line))
                    {
                        if(line.length() == 7)
                        {
                            paletteIndex[line.substr(2,5)] = std::stoi(line.substr(0,1));
                        }
                        else if(line.length() == 8)
                        {
                            paletteIndex[line.substr(3,6)] = std::stoi(line.substr(0,2));
                        }
                    }
                }

                break;
        }
    }

    



	char temp = 0;
	auto dataSize = ((bmp.infoHeader.width * 3 + 3) & (~3)) * bmp.infoHeader.height;

    std::cout << "Datasize: " << dataSize << "\n";

    // pre-load palette from file is present
    //std::map<std::string, int> paletteIndex;
    /*
	if(argc == 3)
	{
		std::ifstream file(argv[2]);
		if(file.is_open())
		{
            std::string line;
			while(std::getline(file, line))
			{
				if(line.length() == 7)
				{
					paletteIndex[line.substr(2,5)] = std::stoi(line.substr(0,1));
				}
				else if(line.length() == 8)
				{
					paletteIndex[line.substr(3,6)] = std::stoi(line.substr(0,2));
				}
			}
		}
	}
    */

    // make a default color is the palette is empty
	if(paletteIndex.size() == 0)
	{
        switch(type)
        {
        case MEGA_DRIVE:
            paletteIndex["$0000"] = 0;
            break;
        case MASTER_SYSTEM:
            paletteIndex["%00000000"] = 0;
            break;
        case GAMEGEAR:
            paletteIndex["%00001111,%00000000"] = 0;
            break;
        case TMS:
            paletteIndex["%00000000"] = 0;
            break;
        case RGB:
        default:
            paletteIndex["0,0,0"] = 0;
            break;
        }
	}
	int currentPaletteIndex = paletteIndex.size();

	
	int pixelCount = 0;
	int widthCounter = 0;
	int spriteRowCounter = 0;

	int numTiles = (bmp.infoHeader.width * bmp.infoHeader.height) / 64;

    std::vector<std::stringstream> tiles;
	tiles.reserve(numTiles);

    std::cout << "Width: " << bmp.infoHeader.width << "\n";
    std::cout << "Height: " << bmp.infoHeader.height << "\n";
    std::cout << "Num Tiles: " << numTiles << "\n";

	for(int i = 0; i < numTiles; i++)
	{
		tiles.push_back(std::stringstream());
		tiles[i] << std::uppercase << std::hex;
	}
    std::cout << "Tile mem:" << tiles.size() << "\n";
	

    std::cout << "start...\n";

	unsigned char rbSwapTmp;

	for(int i = 0; i < dataSize; i += 3)
	{
		// Swap R and B values
        /*
		if(bmp.data[i] != bmp.data[i+2])
		{
			rbSwapTmp = bmp.data[i];
			bmp.data[i] = bmp.data[i+2];
			bmp.data[i+2] = rbSwapTmp;
		}
        */

        std::string color = 
            FormatColor(
                bmp.data[i+2],
                bmp.data[i+1],
                bmp.data[i],
                type)
            ;

        /*
        std::stringstream ssPalette;
		ssPalette << "$0" << std::uppercase << std::hex <<	
			(int(bmp.data[i+2] & 0xff) >> 4) <<
			(int(bmp.data[i+1] & 0xff) >> 4) <<
			(int(bmp.data[i] & 0xff) >> 4);

        std::string color = ssPalette.str();
        */

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

    std::cout << "\n";

	int numFinalTiles = tiles.size();
	int tilesByWidth = bmp.infoHeader.width / 8;
	int tilesByHeight = bmp.infoHeader.height / 8;

    std::vector<std::string> reorderedTiles;
    std::vector<std::string> bitplaneTiles;
	// Reorder list
	/*
	for(int i = 0; i < numFinalTiles; i += tilesByWidth)
	{
		for(int j = 0; j < tilesByWidth; j++)
		{
			reorderedTiles.push_back(tiles[numFinalTiles-i+j-tilesByWidth].str());
		}
	}
	*/
	
	for(int i = 0; i < tilesByWidth; i++)
	{
		for(int j = tilesByHeight-1; j >= 0; j--)
		{
			reorderedTiles.push_back(tiles[j*tilesByWidth+i].str());
		}
	}
	

	// Format all tiles
	for(int i = 0; i < numFinalTiles; i++)
	{
        std::cout << "raw tile: " << reorderedTiles[i] << "\n";;
        reorderedTiles[i] = FormatTile(reorderedTiles[i], size_t(8));
        if(type == MASTER_SYSTEM)
        {
            reorderedTiles[i] = ConvertPackedTileToBitplane(reorderedTiles[i]);
        }

	}


    std::cout << "Pixel Count: " <<  pixelCount << "\n";
    std::cout << "Tile Count: " << tiles.size() << "\n";

    std::cout << "stop...\n"; 

    std::cout << "palette\n";
    std::multimap<int, std::string> newMap = FlipMap(paletteIndex);
	for(auto const& palette : newMap)
	{
        std::cout << palette.first << "," << palette.second << "\n";
	}

	int c = 1;   
	for (auto const& tile : reorderedTiles)
	{
        std::cout << c << ": \n";
        std::cout << tile << "\n";
        //std::cout << "PACKED:";
        //std::string newTile = ConvertPackedTileToBitplane(tile);
        //std::cout << newTile << "\n";
		c++;
	}

	return 0;
}
