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
			static void parse_url(URL &url);
			static void parseHeader(Response &response);
		~Parser();
	
	private:
        static std::string getHeaderElement(std::string str, std::string matchString);
        static std::string parseHexValue(std::string filename);
};



#endif
