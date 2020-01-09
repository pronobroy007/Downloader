#ifndef STRUCTURE
#define STRUCTURE 

#include <iostream>
#include <fstream>

enum FILE_ENCODING {length, chunked, connection, def};

struct Response
{
	//Encoding type "Chunked" or "Content_Length"
	FILE_ENCODING encoding;
	std::ofstream output;

	//Total file size.
	long fileSize = 0;
	long fileSizeShow = 0;
	//This many byte we write on output file eatch time.
	int byte_write = 0;
	//Eatch time we write on output file we increment this download size.
	long totalDownSize = 0; 

	std::string header;
	std::string fileName;
	std::string fileType;




	void clear()
	{
		encoding = FILE_ENCODING::def;

		if(output.is_open())
			output.close();

		fileSize = 0;
		byte_write = 0;
		totalDownSize = 0;
		fileSizeShow = 0;

		header.clear();
		fileName.clear();
		fileType.clear();
	}

};

#endif
