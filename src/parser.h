#ifndef PARSER
#define PARSER

#include <iostream>
#include <string>
#include <sstream>
#include "structure.h"

class Parser
{
	public:
		Parser();
			void parse_url(std::string url, std::string &hostname, std::string &port, std::string &path, std::string &protocol, std::string &filename);
			void parseHeader(std::string header, std::string &fileType, long &fileSize, FILE_ENCODING &encoding);
		~Parser();
	
	private:
};



#endif
