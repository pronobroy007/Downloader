#include "downloader.h"

Downloader::Downloader()
{
	_encoding = FILE_ENCODING::def;
}

bool Downloader::download(std::string url)
{
	std::string hostname;
	std::string port;
	std::string path;
	std::string protocol;

	//Parser url..............
	parser.parse_url(url, hostname, port, path, protocol);
    std::cout << std::endl;
	std::cout << "**************************************************************"<< std::endl;
    std::cout << "Protocol : " << protocol << std::endl;
    std::cout << "URL : " << url << std::endl;
    std::cout << "Host name : " << hostname << std::endl;
    std::cout << "Port : " << port << std::endl;
    std::cout << "Path : " << path << std::endl;
    std::cout << std::endl;
	
	//Connecting to server........................
	connectServer(hostname, port, protocol);
	std::cout << std::endl;
	std::cout << "**************************************************************\n"<< std::endl;
	send_req(hostname, port, path);
	std::cout << std::endl;

	//Downloading..............................
    char *ptr = (char*)recvBuff;
    while (true)
    {
		//sleep(0.1);
        _byte_recv = recv(sock, ptr, BUFFER_SIZE, 0);
		//std::cout << "Byte received : " << byte_recv << std::endl;

        if(_byte_recv < 1) 
           break;
	
		if(_encoding == FILE_ENCODING::def)
		{
			/******************************************************
			// After gatting response from server only first time
			// we enter here rest will execute "else" part.
			******************************************************/
			if(body = strstr(recvBuff, "\r\n\r\n"))
			{
				// For "\r\n\r\n"
				body = body + 4;
				
				//Printing header.....................
				_header = std::string(recvBuff);
				_header = std::string(_header, 0, _header.find("\r\n\r\n"));
				std::cout << "**************************************************************"<< std::endl;
				std::cout << "Header : " << std::endl;
				std::cout << "----------" << std::endl;
				std::cout << _header << std::endl;
				std::cout << "**************************************************************\n"<< std::endl;
				
				/******************************************************
				// This function sets "enccoding" type.
				// also set "byte_write" and "Content_Type"
				******************************************************/
				parser.parseHeader(_header, _fileType, _fileSize, _encoding);
				fileSizeShow = getSize(_fileSize);
				std::cout << "**************************************************************"<< std::endl;
				std::cout << "File Type : " << _fileType << std::endl;	
				std::cout << "File Length : " << _fileSize << std::endl;
				std::cout << "Bit read : " << _byte_write << std::endl;
				std::cout << "byte recv : " << _byte_recv << std::endl;
				std::cout << "header size : " << _header.size() << std::endl;
				std::cout << "**************************************************************\n"<< std::endl;

				/******************************************************/
				// This many data need to write first time.
				// Because data and header mixed together.
				// so get read of header part form actual data.
				/******************************************************/
				_byte_write = _byte_recv - (_header.size() + 4); 

			}
		}
		else
		{
			/******************************************************
			// This two variable is use to write on file.
			// body is what we are writting and byte_write is size.
			******************************************************/
			body = recvBuff;
			_byte_write = _byte_recv;
		}
		


		//Downloading file.............
		switch(_encoding)
		{
			case FILE_ENCODING::length:
			{
				if(!output.is_open())
				{
					output.open("hello." + _fileType, std::ios::binary);
				}
				//Download when file is given as "Content_Length"
				content_length();
				
				//Make sure program end after download.
				if(totalDownSize >= _fileSize)
				{
					cleanUp();
					return true;
				}
				break;
			}

			case FILE_ENCODING::chunked:
			{
				if(!output.is_open())
				{
					output.open("hello.html");
				}
				//Download when file is given as "Chunked"
				chunked();
				break;	
			}

			default:
			{
				std::cout << "Download type unknown" << std::endl;
				break;
			}	

		}//end_switch
	}//end_while.


	cleanUp();
    std::cout << "File Received !" << std::endl;
    return true;
}




//******************************************************************************************
inline void Downloader::content_length()
{
	//Write on it........................
	totalDownSize += _byte_write;

	if(totalDownSize > _fileSize)
	{
		_byte_write = _byte_recv - (totalDownSize - _fileSize);
		totalDownSize = (totalDownSize - _byte_recv) + _byte_write;
	}

	output.write(body, _byte_write); 
	//KB = 1024
	//MB = 1048576 
	//GB = 1073741824 
	float s = (float)totalDownSize / _fileSize;
	std::cout << "Downloaded : " << s * 100 << " % ------- ";
	std::cout << (int)getSize(totalDownSize) << " / " << fileSizeShow << std::endl;
	//std::cout << _byte_write << "  " << _byte_recv << std::endl;
	//std::cout << body << std::endl;
}

//..........................................................................................

inline void Downloader::chunked()
{
	int remmining_byte = _byte_write;
	while(remmining_byte > 0)
	{
		//Get Chunked size...................
		if(_fileSize == 0)
		{
			//Find length of string.
			std::string str = std::string(body);
			index = str.find("\r\n");
			str = std::string(str, 0, index);
			std::stringstream ss(str);
			ss >> std::hex >> _fileSize;

			body = strstr(body, "\r\n");
			body += 2;

			_byte_write -= (index + 2);
			remmining_byte -= (index + 2);
		}

		//Write on it........................
		totalDownSize += _byte_write;

		if(totalDownSize >= _fileSize)
		{
			_byte_write = _byte_recv - (totalDownSize - _fileSize);
			totalDownSize = (totalDownSize - _byte_recv)	+ _byte_write;
			output.write(body, _byte_write); 
			remmining_byte -= _byte_write;	

			//Reset file size.
			_fileSize = 0;
			totalDownSize = 0;
			body += _byte_write;
			std::cout << " > Remmining byte : " << remmining_byte << std::endl;

			//End of file.............
			if(strstr(body, "\r\n\r\n"))
				remmining_byte = 0;

		}else
		{
			output.write(body, _byte_write); 
			remmining_byte -= _byte_write;	
			std::cout << _byte_write << "  " << _byte_recv << std::endl;
			std::cout << "else Remmining byte : " << remmining_byte << std::endl;
		}
	
	}//end_wile
}

//******************************************************************************************

inline void Downloader::cleanUp()
{
	_byte_recv = 0;
	_byte_write = 0;
	totalDownSize = 0;
	_fileSize = 0;

	body = 0;
	index = 0;

	_header.clear();
	_fileType.clear();

	if(output.is_open())
		output.close();

	_encoding = FILE_ENCODING::def;

	std::cout << "Cleaning up ............." << std::endl;
}


Downloader::~Downloader()
{
	if(output.is_open())
		output.close();

	_byte_write = 0;
	totalDownSize = 0;
	_fileSize = 0;

	_encoding = FILE_ENCODING::def;
}

