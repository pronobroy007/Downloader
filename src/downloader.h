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
		Parser parser;

		//For show.............
		long fileSizeShow = 0;
		int _byte_recv = 0;
		int _byte_write = 0;
		long totalDownSize = 0; 
		long _fileSize = 0;

		char* body;
		size_t index;

		std::string _header;
		std::string _fileType;
		FILE_ENCODING _encoding;
		std::ofstream output;

		//Functions.................
		inline void content_length();
		inline void chunked();
		inline void cleanUp();
};
#endif

