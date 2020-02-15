#ifndef DOWNLOADER 
#define DOWNLOADER 

#include "client.h"
#include "parser.h"
#include "structure.h"
#include "database.h"

class Downloader : public Client
{
	public:
        std::string projectDir;

		Downloader();
            void init(std::string address, URL &url);
			bool download(URL &url, long strartingByte = 0);
            void cleanUp();
		~Downloader();

	private:
		char* body;
		//Parser parser;
		Response _response;
        Database db;

		//Functions.................
		inline void encodingChecker();
		inline void content_length();
		inline bool chunked(std::string protocol);
};
#endif

