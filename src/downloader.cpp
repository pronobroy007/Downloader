#include "downloader.h"

Downloader::Downloader()
{
	_response.encoding = FILE_ENCODING::def;
}

bool Downloader::download(std::string url)
{
	std::string hostname;
	std::string port;
	std::string path;
	std::string protocol;

	//Parser url..............
	parser.parse_url(url, hostname, port, path, protocol, _response.fileName);
	if(_response.fileName.empty())
		_response.fileName = "hello";
	//
	//Connecting to server........................
	connectServer(hostname, port, protocol);
	send_req(hostname, port, path);

	
	//Print url parse.
    std::cout << std::endl;
	std::cout << "**************************************************************"<< std::endl;
    std::cout << "Protocol : " << protocol << std::endl;
    std::cout << "URL : " << url << std::endl;
    std::cout << "Host name : " << hostname << std::endl;
    std::cout << "Port : " << port << std::endl;
    std::cout << "Path : " << path << std::endl;
    std::cout << "File Name : " << _response.fileName << std::endl;
    std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "**************************************************************\n"<< std::endl;
	//Print Request.
	std::cout << "**************************************************************"<< std::endl;
    std::cout << "Rsquest : " << std::endl;
	std::cout << "---------"<< std::endl;
	std::cout << sendBuff << std::endl;
	std::cout << "**************************************************************"<< std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	//Downloading..............................
    char *ptr = (char*)recvBuff;
    while (true)
    {
		int byte_recv;
		usleep(10);
		if(protocol == "http")
			byte_recv = recv(sock, ptr, BUFFER_SIZE, 0);


        if(byte_recv < 1) 
           break;
	
		if(_response.encoding == FILE_ENCODING::def)
		{
			/******************************************************
			// After gatting response from server only first time
			// we enter here rest will execute "else" part.
			******************************************************/
			if((body = strstr(recvBuff, "\r\n\r\n")))
			{
				// For "\r\n\r\n"
				body = body + 4;
				
				_response.header = std::string(recvBuff);
				_response.header = std::string(_response.header, 0, _response.header.find("\r\n\r\n"));
				
				/******************************************************
				// This function sets "enccoding" type.
				// also set "byte_write" and "Content_Type"
				******************************************************/
				parser.parseHeader(_response.header, _response.fileType, _response.fileSize, _response.encoding);
				_response.fileSizeShow = getSize(_response.fileSize);
				
				//Printing header.....................
				std::cout << "**************************************************************"<< std::endl;
				std::cout << "Header : " << std::endl;
				std::cout << "----------" << std::endl;
				std::cout << _response.header << std::endl;
				std::cout << "**************************************************************\n"<< std::endl;
				//Printing Response.
				std::cout << "**************************************************************"<< std::endl;
				std::cout << "File Type : " << _response.fileType << std::endl;	
				std::cout << "File Length : " << _response.fileSize << std::endl;
				std::cout << "Bit read : " << _response.byte_write << std::endl;
				std::cout << "byte recv : " << byte_recv << std::endl;
				std::cout << "header size : " << _response.header.size() << std::endl;
				std::cout << "**************************************************************\n"<< std::endl;

				/******************************************************/
				// This many data need to write first time.
				// Because data and header mixed together.
				// so get read of header part form actual data.
				/******************************************************/
				_response.byte_write = byte_recv - (_response.header.size() + 4); 

			}
		}
		else
		{
			/******************************************************
			// This two variable is use to write on file.
			// body is what we are writting and byte_write is size.
			******************************************************/
			body = recvBuff;
			_response.byte_write = byte_recv;
		}
		


		//Downloading file.............
		switch(_response.encoding)
		{
			case FILE_ENCODING::length:
			{
				if(!_response.output.is_open())
				{
					_response.output.open("download/" + _response.fileName, std::ios::binary);
				}
				//Download when file is given as "Content_Length"
				content_length(byte_recv);
				
				//Make sure program end after download.
				if(_response.totalDownSize >= _response.fileSize)
				{
					cleanUp();
					return true;
				}
				break;
			}

			case FILE_ENCODING::chunked:
			{
				if(!_response.output.is_open())
				{
					_response.output.open("download/" + _response.fileName);
				}
				//Download when file is given as "Chunked"
				chunked(byte_recv);
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
inline void Downloader::content_length(int byte_recv)
{
	//Write on it........................
	_response.totalDownSize += _response.byte_write;

	if(_response.totalDownSize > _response.fileSize)
	{
		_response.byte_write = byte_recv - (_response.totalDownSize - _response.fileSize);
		_response.totalDownSize = (_response.totalDownSize - byte_recv) + _response.byte_write;
	}

	_response.output.write(body, _response.byte_write); 
	float s = (float)_response.totalDownSize / _response.fileSize;
	std::cout << "Downloaded : " << s * 100 << " % ------- ";
	std::cout << (int)getSize(_response.totalDownSize) << " / " << _response.fileSizeShow << std::endl;
	std::cout << _response.fileName << std::endl;
}

//..........................................................................................

inline void Downloader::chunked(int byte_recv)
{
	int remmining_byte = _response.byte_write;
	while(remmining_byte > 0)
	{
		//Get Chunked size...................
		if(_response.fileSize == 0)
		{
			//Find length of string.
			std::string str = std::string(body);
			size_t index = str.find("\r\n");
			str = std::string(str, 0, index);
			std::stringstream ss(str);
			ss >> std::hex >> _response.fileSize;

			body = strstr(body, "\r\n");
			body += 2;

			_response.byte_write -= (index + 2);
			remmining_byte -= (index + 2);
		}

		//Write on it........................
		_response.totalDownSize += _response.byte_write;

		if(_response.totalDownSize >= _response.fileSize)
		{
			_response.byte_write = byte_recv - (_response.totalDownSize - _response.fileSize);
			_response.totalDownSize = (_response.totalDownSize - byte_recv)	+ _response.byte_write;
			_response.output.write(body, _response.byte_write); 
			remmining_byte -= _response.byte_write;	

			//Reset file size.
			_response.fileSize = 0;
			_response.totalDownSize = 0;
			body += _response.byte_write;
			std::cout << " > Remmining byte : " << remmining_byte << std::endl;

			//End of file.............
			if(strstr(body, "\r\n\r\n"))
				remmining_byte = 0;

		}else
		{
			_response.output.write(body, _response.byte_write); 
			remmining_byte -= _response.byte_write;	
			std::cout << _response.byte_write << "  " << byte_recv << std::endl;
			std::cout << "else Remmining byte : " << remmining_byte << std::endl;
		}
	
	}//end_wile
}

//******************************************************************************************

inline void Downloader::cleanUp()
{
	body = 0;
	_response.clear();

	std::cout << "Cleaning up ............." << std::endl;
}


Downloader::~Downloader()
{
	body = 0;
	_response.clear();
}

