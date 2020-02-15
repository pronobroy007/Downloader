#include "parser.h"

Parser::Parser() {}

void Parser::parseHeader(Response &response)
{
	ssize_t index;
    
	//Get file name.......................
    //*********************************************************************
    std::string filename = getHeaderElement(response.header, "filename=\"");	
    if(filename != "null")
        response.fileName = std::string(filename, 0, filename.size()-1);
    //*********************************************************************

	//Get file type.......................
    //*********************************************************************
    response.fileType = getHeaderElement(response.header, "Content-Type:");
    //*****************************************
    
    //Accept-Ranges:
    //*********************************************************************
    response.acceptRange = getHeaderElement(response.header, "Accept-Ranges:");
    //*********************************************************************
    
	//Content_Length.......................
    //*********************************************************************
    std::string sLen = getHeaderElement(response.header, "Content-Length: ");
    if(sLen != "none")
    {
		//Convaart string to int.
		std::stringstream ss(sLen);
		ss >> response.fileSize;

		response.encoding = FILE_ENCODING::length;
	}
    //*********************************************************************
	
	//Chunked........................
    //*****************************************
	index = response.header.find("Transfer-Encoding: chunked");
	if(index > 0)
		response.encoding = FILE_ENCODING::chunked;	
    //*****************************************
    
    //Connection...........................
    //*****************************************
    if(response.encoding == FILE_ENCODING::def)
        response.encoding = FILE_ENCODING::connection;
    //*****************************************
    
    //Printing header.....................
    std::cout << "**************************************************************"<< std::endl;
    std::cout << "Header : " << std::endl;
    std::cout << "----------" << std::endl;
    std::cout << response.header << std::endl;
    std::cout << "**************************************************************\n"<< std::endl;
    //Printing Response.
    std::cout << "**************************************************************"<< std::endl;
    std::cout << "File name : " << response.fileName << std::endl;	
    std::cout << "File Type : " << response.fileType << std::endl;	
    std::cout << "Accept Ranges : " << response.acceptRange << std::endl;	
    std::cout << "File Length : " << response.fileSize << std::endl;
    std::cout << "header size : " << response.header.size() << std::endl;
    std::cout << "**************************************************************\n"<< std::endl;
    
}


void Parser::parse_url(URL &url)
{
    std::string newUrl;

	//Get File name.
    //*****************************************
	size_t p = url.url.rfind("/");
	url.fileName = std::string(url.url, p+1, url.url.size());
    url.fileName = parseHexValue(url.fileName);
    //*****************************************

	//Get protocol like 'http' or 'https'.
    //*****************************************
    p = url.url.find("://");
	url.protocol = std::string(url.url, 0, p);

    if(p > 0)
        newUrl.assign(url.url, p+3, url.url.size());
    else 
        newUrl = url.url;
    //*****************************************

    
	//Get hostname like youtube.com
    //*****************************************
    p = 0;
    p = newUrl.find("/");
    if(p > 0)
    {
        url.hostname.assign(newUrl, 0, p--);
        newUrl.assign(newUrl, p+2, newUrl.size());

        //Get path................
        p = newUrl.find("#");
        if(p)
            url.path.assign(newUrl, 0, p);
        else
            url.path = newUrl;
    }else
        url.hostname = newUrl;
    //*****************************************
    

    //update hostname like if we have localhost:5555
	//Then our hostname = localhost and port = 5555
    //Or make default port for http = 80 and https = 443
    //*****************************************
    ssize_t np = url.hostname.find(":");
    if(np > 0)
    {
        url.port.assign(url.hostname, np+1, url.hostname.size() - np + 1);
        url.hostname.assign(url.hostname, 0, np);
    }else
    {
        if(url.protocol == "http")
            url.port = "80";
        else if(url.protocol == "https")
            url.port = "443";
    }
    //*****************************************
    
	//Print url parse.
    std::cout << std::endl;
	std::cout << "**************************************************************"<< std::endl;
    std::cout << "Protocol : " << url.protocol << std::endl;
    std::cout << "URL : " << url.url << std::endl;
    std::cout << "Host name : " << url.hostname << std::endl;
    std::cout << "Port : " << url.port << std::endl;
    std::cout << "Path : " << url.path << std::endl;
    std::cout << "File Name : " << url.fileName << std::endl;
    std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "**************************************************************\n"<< std::endl;
}



std::string Parser::getHeaderElement(std::string str, std::string matchString)
{
    std::string element = "null";
	int index = str.find(matchString);

	if(index > 0)
	{
		element = std::string(str, index+matchString.size());	
		index = element.find("\r\n");
		element = std::string(element, 0, index);
	}
    
    return element;
}


std::string Parser::parseHexValue(std::string filename)
{
    /*********************************************************/
    //  Remove %hex_value to ascii form http get request
    /*********************************************************/
    ssize_t index = 0;
    do
    {
        index = filename.find("%", index);
        if(index == std::string::npos)
            index = 0;
        else
        {
            //Here val have hex number.
            std::string val(filename, index+1, 2);
            std::stringstream s(val);
            int integer;
            s >> std::hex >> integer;
            val = char(integer);

            filename.erase(index, 3);
            filename.insert(index, val);
        }
    }while(index);
    /*********************************************************/

    return filename;

}

Parser::~Parser() {}

