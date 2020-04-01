#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <unordered_set>
using std::unordered_set;
#include <sstream>
using std::stringstream;

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

	std::unordered_set<string> palette;

	//DC.W    $0000,$0044,$0066,$0088

	for (auto i = dataSize - 3; i >= 0; i -= 3)
	{
		temp = bmp.data[i];
		bmp.data[i] =  bmp.data[i+2];
		bmp.data[i+2] = temp;

		stringstream ss;
		ss << "$0" << std::uppercase << std::hex <<	
			(int(bmp.data[i+2] & 0xff) >> 4) <<
			(int(bmp.data[i+1] & 0xff) >> 4) <<
			(int(bmp.data[i] & 0xff) >> 4);
		palette.insert(ss.str());
}

	for(auto color : palette)
	{
		cout << color << endl;
	}
	
	return 0;
}
