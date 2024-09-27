#include <iostream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <iterator>


#include <iomanip>

#include "../shared/img/bitmap.h"
#include "../shared/img/sega_img.h"

#include "img-objects.h"

PatternTable LoadBitmap(const char* filename);
PatternTable ReorderBitmapToTiles(PatternTable pt);
void ConvertToPlanar(uint8_t arr[8], uint8_t* result);


PatternTable LoadBitmap(const char* filename)
{
    Bitmap bmp;
    bmp.Read(filename);
    auto dataSize = ((bmp.infoHeader.width * 3 + 3) & (~3)) * bmp.infoHeader.height;
   

    PatternTable pt;
    Pattern p;
    p.label = "import bmp";
    pt.h = bmp.infoHeader.height;
    pt.w = bmp.infoHeader.width;

    // Bitmap data is stored from the bottom-left to top right
    // ... re-order the rows from top-left to bottom-right
    int16_t rowSize = bmp.infoHeader.width * 3; // 3 bytes (RGB) times width

    // Reverse all rows (Loop over each row up to the middle)
    for (int16_t i = 0; i < bmp.infoHeader.height / 2; ++i)
    {
        int16_t topRowIndex = i * rowSize;
        int16_t bottomRowIndex = (bmp.infoHeader.height - 1 - i) * rowSize;

        // Swap pixels between the top and bottom rows using XOR swap
        for (int j = 0; j < rowSize; ++j)
        {
            bmp.data[topRowIndex + j] ^= bmp.data[bottomRowIndex + j];
            bmp.data[bottomRowIndex + j] ^= bmp.data[topRowIndex + j];
            bmp.data[topRowIndex + j] ^= bmp.data[bottomRowIndex + j];
        }
    }



    std::map<RgbColor,int16_t> reverseColorLookup;
    Palette palette;
    int16_t nextIndexColor = 0;
    //std::cout << "Datasize: " << dataSize
    //    << " Pixels: " << (dataSize/3) << "\n";

    for(int16_t i = 0; i < dataSize; i += 3) // loop all pixels, 3 bytes at a time (RGB)
    {
        // NOTE: Bitmaps are BGR format, flip to RGB
        RgbColor c = {bmp.data[i+2],bmp.data[i+1],bmp.data[i]}; // make a color from the pixel data
        // Try to find paletteId, add a new one if it's not found
        if(reverseColorLookup.find(c) == reverseColorLookup.end())
        {
            reverseColorLookup[c]=nextIndexColor; 
            palette.colors[nextIndexColor] = c;
            // Output index colors as they are added
            //std::cout << "Added " << int(nextIndexColor) << ": " 
            //    << int(c.r) << ", " 
            //    << int(c.g) << "," 
            //    << int(c.b) << "\n";
            nextIndexColor++;
        }
        // Add pixel to pattern (by adding paletteId)
        int16_t colorIndex = reverseColorLookup[c]; // Look up paletteId from palette
        p.paletteIds.push_back(colorIndex); // Add paletteId for pixel
        //std::cout << ".";
    }
    // Add Pattern to PatternTable
    pt.patterns.push_back(p);

    // Output Palette
    //std::cout << "Id count: " << p.paletteIds.size() << "\n";
    //std::cout << "\n";
    //std::cout << "Palette: \n";
    //for(const auto& pal : palette.colors)
    //{
    //    std::cout << int(pal.first) << ": "
    //        << int(pal.second.r) << ","
    //        << int(pal.second.g) << ","
    //        << int(pal.second.b) << "\n";
    //}
    //std::cout << "\n\n";
    
    // Output Pattern
    //std::cout << "Pattern:\n";
    int counter = 0;

    //for(const auto& paletteId : p.paletteIds)
    //{
    //    if(counter%8 == 0)
    //    {
    //        std::cout << "\n";
    //    }
    //    counter++;

    //    std::cout << int(paletteId) << ",";
    //}
    //std::cout << "\n\n";

    //std::cout << "ReorderBitmapToTiles\n";

    return pt;
}
    
// CHUNK BITMAP TO TILES ////////
//    PatternTable tiledTable = ReorderBitmapToTiles(pt);
std::list<uint8_t> ConvertToSmsFormat(const PatternTable& tiledTable)
{   
    // CONVERT TO SMS FORMAT ////////
    // NOTE: ONLY works for SMS, assumes 8x8 pixel tiles
    uint8_t numTiles = tiledTable.patterns.size();
    std::list<uint8_t> smsResult;
    for(auto const& tile : tiledTable.patterns) // loop all tiles
    {
        //std::cout << "t";
        for(uint8_t i=0;i<64;i+=8) // loop all rows
        {
            uint8_t rowArr[8];
            for(uint8_t j = 0; j<8;j++) // add each pixel in row to array
            {
                rowArr[j] = tile.paletteIds[i+j];
            }
            
            uint8_t resultArr[4];
            ConvertToPlanar(rowArr, resultArr); // result size 4 objects
            for(uint8_t j = 0; j<4;j++)
            {
                smsResult.push_back(resultArr[j]);
            }
        }
    }
   
    return smsResult; 
}

void OutputToSmsFormat(const std::list<uint8_t>& smsResult)
{  
    // OUTPUT SMS RESULT
    std::cout << "SmsResult:\n";

    int16_t chunkCounter = 0;
    for(auto const& byteChunk : smsResult)
    {
        if(chunkCounter%32 == 0)// new tile comment
        {
            std::cout << "\n  ;" << int(chunkCounter/32) << "\n";
        }

        if(chunkCounter%4 == 0)
        {
            std::cout << "  db $";
        }

        std::cout << std::uppercase 
            << std::hex 
            << std::setw(2) 
            << std::setfill('0') 
            << (int)byteChunk; 

        std::cout << (((chunkCounter+1)%4==0) ? "\n" : ",$");


        chunkCounter++;
    }
    
    std::cout << "\nSmsResultEnd:\n";
}

PatternTable ReorderBitmapToTiles(PatternTable pt)
{
    PatternTable newPatternTable;
    newPatternTable.w = 8;
    newPatternTable.h = 8;
    int numTiles = (pt.w*pt.h) / 64;

    uint16_t pixelCount = 0;
    uint16_t widthCounter = 0;
    uint16_t spriteRowCounter=0;

    //std::cout << "Pixels: ";

    // Loop all tiles (increment by tile width and height each time)
    for(int tileYCounter=0; tileYCounter < pt.h; tileYCounter += newPatternTable.h)
    {
        for(int tileXCounter=0; tileXCounter < pt.w; tileXCounter += newPatternTable.w)
        {
            Pattern p;
            // Loop tile width and height
            for(int i=0; i<newPatternTable.w; i++) // width
            {
                for(int j=0; j<newPatternTable.h; j++) // height
                {
                    // Get pixel number
                    int pixelNum =  
                        ( (tileYCounter+i) * pt.w) // row
                        + (tileXCounter+j ) // column
                    ;
                    // lookup paletteId for pixel number
                    uint16_t paletteId = pt.patterns[0].paletteIds[pixelNum];
                    //std::cout << int(paletteId) << ",";

                    // add paletteId
                    p.paletteIds.push_back(paletteId);
                }
            }
            newPatternTable.patterns.push_back(p);
        }
    }

    //std::cout << "\n";
    //std::cout << "Tile count: " << newPatternTable.patterns.size() << "\n";
    //std::cout << "New Pattern List:\n";

    uint8_t numPixels = newPatternTable.w*newPatternTable.h;
    //for(int i=0;i<numTiles;i++)
    //{
    //    std::cout << "Tile " << i << ": ";
    //    for(int j=0;j<numPixels;j++)
    //    {
    //        std::cout << newPatternTable.patterns[i].paletteIds[j] << ",";
    //    }
    //    std::cout << "\n";
    //}

    return newPatternTable;
}

/*
void ResizePatternExtents(PatternTable pt, int16_t newWidth, int16_t newHeight)
{
    int16_t patternIds[pt.patterns.size()*pt.w*pt.h];
    // Resize pa
    int i = 0;
    for(const auto& p : pt.patterns) // loop each tile
    {
        for(const auto& id : p.paletteIds)
        {
           patternIds[i]=id; 
        }
    }
}
*/

// Pull the [columnNum]th bit from an array of 8 bytes and combine to a single byte
uint8_t PullBitsByColumn(uint8_t arr[8], uint8_t columnNum)
{
    uint8_t result = 0;
    for(uint8_t i = 0; i<8; i++)
    {
        uint8_t bit = (arr[i] >> columnNum)&1; // pull bit and mask off
        result |= (bit << (7-i)); // move bit back to inverse position and combine with result
    }

    return result;
}

// Convert tiles to Planar (SMS) format, note, only the last nibble of the input is used (16 colors)
void ConvertToPlanar(uint8_t arr[8], uint8_t* result) // result size 4 objects
{
    for(uint8_t i=0; i<4 ;i++) // do 4 times (each nibble is 4 bits)
    {
        result[i] = PullBitsByColumn(arr,i);
    }
}

void MoveColor(uint16_t from, uint16_t to, PatternTable& tilesTable)
{
    for(auto& p : tilesTable.patterns)
    {
        for(auto& id : p.paletteIds)
        {
            if(id == from)
            {
                id = to;
            }
        }
    }
}

// CHUNK BITMAP TO TILES ////////
//    PatternTable tiledTable = ReorderBitmapToTiles(pt);
std::list<uint8_t> ConvertToMdFormat(const PatternTable& tiledTable)
{   
    // CONVERT TO MD FORMAT ////////
    // NOTE: assumes 8x8 pixel tiles
    uint8_t numTiles = tiledTable.patterns.size();
    std::list<uint8_t> mdResult;
    for(auto const& tile : tiledTable.patterns) // loop all tiles
    {
        //for(auto const& id : tile.paletteIds)
        //{
        //    mdResult.push_back(id);
        //}

        for (size_t i = 0; i < tile.paletteIds.size(); i += 2)
        {
            uint8_t byte = 0;

            // Take the first nibble and shift it to the high nibble position.
            byte = (tile.paletteIds[i] & 0x0F) << 4;

            // If there is a second nibble, combine it with the low nibble of the byte.
            if (i + 1 < tile.paletteIds.size())
            {
                byte |= (tile.paletteIds[i + 1] & 0x0F);
            }

            // Add the combined byte to the second list.
            mdResult.push_back(byte);
        }
    }

   
    return mdResult; 
}

void OutputToMdFormat(const std::list<uint8_t>& mdResult)
{  
    // OUTPUT MD RESULT
    std::cout << "MdResult:\n";

    int16_t chunkCounter = 0;
    for(auto const& byteChunk : mdResult)
    {
        if(chunkCounter%32 == 0)// new tile comment
        {
            std::cout << "\n  ;" << int(chunkCounter/32) << "\n";
        }

        if(chunkCounter%4 == 0)
        {
            std::cout << "  db $";
        }

        std::cout << std::uppercase 
            << std::hex 
            << std::setw(2) 
            << std::setfill('0') 
            << (int)byteChunk; 

        std::cout << (((chunkCounter+1)%4==0) ? "\n" : ",$");


        chunkCounter++;
    }
    
    std::cout << "\nMdResultEnd:\n";
}





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
    std::map<std::string, int> paletteIndex;

    // Load bitmap
    //PatternTable bitmapTable = LoadBitmap("../target_pattern_test.bmp");
    //LoadBitmap("../keypad_map.bmp");
    //PatternTable bitmapTable = LoadBitmap("../number_test.bmp");

    // Load the bitmap into a pattern table
    PatternTable bitmapTable = LoadBitmap("../keypad_map.bmp");
    // Reorganize the pattern bitmap into a new table with 8x8 patterns
    PatternTable tiledTable = ReorderBitmapToTiles(bitmapTable);

    // TODO 
    // SwapColors(uint16_t a, uint16_t b, tilesTable); // swap two indexes
    // MoveColor(uint16_t a, PatternTable tilesTable); // change one index to another
    MoveColor(0,2,tiledTable); 


    std::cout << "Sms_Pattern_Keypad:\b";
    // Create a Planar formated list of bytes for the SMS
    std::list<uint8_t> smsResult1 = ConvertToSmsFormat(tiledTable);
    // Print the SMS format as ASM
    //OutputToSmsFormat(smsResult1);


    std::cout << "Sms_Pattern_Keypad_Highlight:\b";
    //MoveColor(2,3,tiledTable); 
    std::list<uint8_t> smsResult2 = ConvertToSmsFormat(tiledTable);
    //OutputToSmsFormat(smsResult2);


    std::cout << "Sms_Pattern_Keypad_Selected:\b";
    //MoveColor(3,4,tiledTable); 
    std::list<uint8_t> smsResult3 = ConvertToSmsFormat(tiledTable);
    //OutputToSmsFormat(smsResult3);




    std::cout << "\n========\n";

    std::list<uint8_t> mdResult = ConvertToMdFormat(tiledTable);
    OutputToMdFormat(mdResult);


/*
    // AEDDEEED
    uint8_t rowArr[8] = {0x0A,0x0E,0x0D,0x0D,0x0E,0x0E,0x0E,0x0D};


    std::cout << "EXPECTED\n31CE7FFF \n";

    uint8_t resultArr[4];
    ConvertToPlanar(rowArr, resultArr); // result size 4 objects
    for(uint8_t j = 0; j<4;j++)
    {
        std::cout << std::uppercase 
            << std::hex 
            << std::setw(2) 
            << std::setfill('0') 
            << (int)resultArr[j]; 

        //smsResult.push_back(resultArr[j]);
    }

    std::cout << "\nEXPECTED BIN\n00110001 11001110 01111111 11111111\n";

    for(uint8_t j = 0; j<4;j++)
    {
        std::bitset<8> binaryValue(resultArr[j]);
        std::cout << binaryValue << " ";
        //smsResult.push_back(resultArr[j]);
    }
*/


    std::cout << "\nDONE\n";




    enum MENU_MODE 
    {
        FILE = 0,
        PALETTE,
        PATTERN
    };
    MENU_MODE mode = MENU_MODE::FILE;
/*
    do
    {
        switch(mode)
        {
        case FILE:
            break;
        case PALETTE:
            break;
    
        }
    }
    while(true);
*/




	return 0;
}
