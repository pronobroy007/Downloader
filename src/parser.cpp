#include "parser.h"

Parser::Parser() {}


void Parser::parseHeader(std::string header, std::string &fileType, long &fileSize, FILE_ENCODING &encoding)
{
	ssize_t index;
	//Get file type.......................
	index = header.find("Content-Type:");
	if(index > 0)
	{
		//13
		fileType = std::string(header, index+13);	
		index = fileType.find("/");
		fileType = std::string(fileType, index+1);
		index = fileType.find("\r\n");
		fileType = std::string(fileType, 0, index);
	}
	//Encoding type and file size...........................
	index = header.find("Content-Length:");
	if(index > 0)
	{
		encoding = FILE_ENCODING::length;
		//"Content-Length:" = 16 char for parse only size.
		std::string sLen = std::string(header, index + 16);
		index = sLen.find("\r\n");
		sLen = std::string(sLen, 0, index);
		sLen.push_back(0);

		//Convaart string to int.
		std::stringstream ss(sLen);
		ss >> fileSize;
		//std::cout << " Lend : "<< sLen << std::endl;
	}
	
	//Chunked........................
	index = header.find("Transfer-Encoding: chunked");
	if(index > 0)
	{
		encoding = FILE_ENCODING::chunked;	
	}

}



void Parser::parse_url(std::string url, std::string &hostname, std::string &port, std::string &path, std::string &protocol, std::string &filename)
{
    std::string newUrl;

	//Get File name.
	size_t p = url.rfind("/");
	filename = std::string(url, p+1, url.size());
	for(int i = 0; i < filename.size(); i++)
		if(filename[i] == '%')
			filename[i] = ' ';	

	//Get protocol like 'http' or 'https'.
    p = url.find("://");
	protocol = std::string(url, 0, p);

    if(p > 0)
        newUrl.assign(url, p+3, url.size());
    else 
        newUrl = url;

    
	//Get hostname like youtube.com
    p = 0;
    p = newUrl.find("/");
    if(p > 0)
    {
        hostname.assign(newUrl, 0, p--);
        newUrl.assign(newUrl, p+2, newUrl.size());

        //Get path................
        p = newUrl.find("#");
        if(p)
            path.assign(newUrl, 0, p);
        else
            path = newUrl;
    }else
        hostname = newUrl;
    

    //update hostname like if we have localhost:5555
	//Then our hostname = localhost and port = 5555
    ssize_t np = hostname.find(":");
    if(np > 0)
    {
        hostname.assign(hostname, 0, np);
        port.assign(hostname, np+1, hostname.size()+1);
    }
	else
		port = "80";


}


Parser::~Parser() {}

