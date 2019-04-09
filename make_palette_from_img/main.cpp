#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <unordered_set>
using std::unordered_set;


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

	for (auto i = dataSize - 3; i >= 0; i -= 3)
	{
		temp = bmp.data[i];
		bmp.data[i] =  bmp.data[i+2];
		bmp.data[i+2] = temp;

		string blah = 
			std::to_string(int(bmp.data[i] & 0xff)) + "," + 
			std::to_string(int(bmp.data[i+1] & 0xff)) + "," + 
			std::to_string(int(bmp.data[i+2] & 0xff));
		palette.insert(blah);
}

	for(auto color : palette)
	{
		cout << color << endl;
	}
	
	return 0;
}
