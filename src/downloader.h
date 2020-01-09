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

		//Functions.................
		inline void content_length(int byte_recv);
		inline void chunked(int byte_recv);
		inline void cleanUp();
};
#endif

