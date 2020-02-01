#ifndef DOWNLOADER 
#define DOWNLOADER 

#include "client.h"
#include "parser.h"
#include "structure.h"

class Downloader : public Client
{
	public:
		Downloader();
			bool download(std::string url);
		~Downloader();

	private:
		char* body;
		Parser parser;
		Response _response;
        std::string projectDir;

		//Functions.................
		inline void content_length();
		inline void chunked();
		inline void cleanUp();
};
#endif

