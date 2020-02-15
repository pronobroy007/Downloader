#ifndef STRUCTURE
#define STRUCTURE 

#include <iostream>
#include <fstream>

enum FILE_ENCODING {length, chunked, connection, def};

struct URL
{
	std::string fileName;
	std::string url;
	std::string hostname;
	std::string port;
	std::string path;
	std::string protocol;

    bool resumeDownload = false;
    std::string downloadID;
};

struct Response
{
	//Encoding type "Chunked" or "Content_Length"
	FILE_ENCODING encoding;
	std::string header;
	std::string fileName;
	std::string fileType;
	std::string acceptRange;
	//Total file size.
	long fileSize = 0;

	std::ofstream output;
	long fileSizeShow = 0;
	//This many byte we write on output file eatch time.
	int byte_write = 0;
	//Eatch time we write on output file we increment this download size.
	long totalDownSize = 0; 

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
		acceptRange.clear();
	}

    long getFileSize(std::string path)
    {
        long size = 0;
        std::ifstream input;
        input.open(path, std::ios::ate);
        if(!input.is_open())
        {
            perror("Faield to open file");
            return 0; 
        }
        
        size = input.tellg();
        input.close();
        return size; 
    }

};

#endif
