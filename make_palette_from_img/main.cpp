#include <iostream>
//#include <string>
#include <cstring>
#include <unordered_set>
#include <sstream>
#include <bitset>

#include <unistd.h>

#include "../shared/img/bitmap.h"
#include "../shared/img/sega_img.h"

int main(int argc, char** argv)
{
    /*
	if(argc != 2)
	{
		std::cerr << "Usage: " << argv[0] 
			<< " filename.bmp" << std::endl;
		return 1;
	}
    */
    //char typeChar = '';
    //std::string filename;
    int opt;
    PaletteType type;
    Bitmap bmp;

    while((opt = getopt(argc, argv, "f:t:")) != -1)
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
        }
    }


    //PaletteType type = GAMEGEAR;
	//Bitmap bmp;
	//bmp.Read(argv[1]);

	char temp = 0;
	auto dataSize = ((bmp.infoHeader.width * 3 + 3) & (~3)) * bmp.infoHeader.height;

	std::unordered_set<std::string> palette;

	for (auto i = dataSize - 3; i >= 0; i -= 3)
	{
        palette.insert(
            FormatColor(
                bmp.data[i+2],
                bmp.data[i+1],
                bmp.data[i],
                type)
            );
    }

	for(auto color : palette)
	{
        std::cout << color << "\n";
	}
	
	return 0;
}
