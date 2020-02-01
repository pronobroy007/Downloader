#include "downloader.h"

Downloader::Downloader()
{
	_response.encoding = FILE_ENCODING::def;
    projectDir = "/home/linux/Documents/program/app/downloader/";
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
		_response.fileName = "index.html";
    
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
    
	//Connecting to server........................
	connectServer(hostname, port, protocol);
    send_req(hostname, port, path);

	
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
    int p = 0;
    while (true)
    {
		int byte_recv;

		usleep(50);
        if(protocol == "https")
            byte_recv = SSL_read(_ssl, ptr + p, BUFFER_SIZE - p);
        else
			byte_recv = recv(sock, ptr + p, BUFFER_SIZE - p, 0);

        if(byte_recv < 1) 
           break;
	
		if(_response.encoding == FILE_ENCODING::def)
		{
            //This is because header may not come in onec.
            p += byte_recv;

			/******************************************************
			// After gatting response from server only first time
			// we enter here rest will execute "else" part.
			******************************************************/
			if((body = strstr(recvBuff, "\r\n\r\n")))
            {	
                body += 4;
				/******************************************************
				// This function sets "enccoding" type.
				// also set "byte_write" and "Content_Type"
				******************************************************/
                _response.header = std::string(recvBuff);
                ssize_t index = _response.header.find("\r\n\r\n");
                _response.header = std::string(recvBuff, 0, index);

				parser.parseHeader(_response.header, _response.fileType, _response.fileSize, _response.encoding);
				_response.fileSizeShow = getSize(_response.fileSize);
                
				/******************************************************/
				// This many data need to write first time.
				// Because data and header mixed together.
				// so get read of header part form actual data.
				/******************************************************/
				_response.byte_write = p - (_response.header.size() + 4); 
                
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
				std::cout << "Byte write : " << _response.byte_write << std::endl;
				std::cout << "byte recv : " << byte_recv << std::endl;
				std::cout << "recvBuff size : " << p << std::endl;
				std::cout << "header size : " << _response.header.size() << std::endl;
				std::cout << "**************************************************************\n"<< std::endl;
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
        if(body)
        {
            //Onece we find our html body no need to track.
            switch(_response.encoding)
            {
                case FILE_ENCODING::length:
                {
                    if(!_response.output.is_open())
                    {
                        _response.output.open(projectDir + "/download/" + _response.fileName, std::ios::binary);
                    }
                    //Download when file is given as "Content_Length"
                    content_length();
                    
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
                        _response.output.open(projectDir + "download/" + _response.fileName);
                    }
                    //Download when file is given as "Chunked"
                    if(_response.byte_write > 0)
                        chunked();
                    break;	
                }

                case FILE_ENCODING::connection:
                {
                    if(!_response.output.is_open())
                    {
                        _response.output.open(projectDir + "download/" + _response.fileName);
                    }
                    //Download when file is given as "Chunked"
                    _response.output.write(body, _response.byte_write); 
                    break;	
                }

                default:
                {
                    std::cout << "Download type unknown" << std::endl;
                    break;
                }	

            }//end_switch
            p = 0;
            bzero(recvBuff, BUFFER_SIZE);
        }
	}//end_while.

	

	cleanUp();
    std::cout << "File Received !" << std::endl;
    return true;
}




//******************************************************************************************
inline void Downloader::content_length()
{
	//Write on it........................
	_response.totalDownSize += _response.byte_write;

	if(_response.totalDownSize > _response.fileSize)
	{
		_response.byte_write = _response.byte_write - (_response.totalDownSize - _response.fileSize);
		_response.totalDownSize = (_response.totalDownSize - _response.byte_write) + _response.byte_write;
	}

	_response.output.write(body, _response.byte_write); 

    //For printing download percentage.
	float s = (float)_response.totalDownSize / _response.fileSize;
	std::cout << "Downloaded : " << s * 100 << " % ------- ";
	std::cout << (int)getSize(_response.totalDownSize) << " / " << _response.fileSizeShow << std::endl;
	std::cout << _response.fileName << std::endl;
}

//..........................................................................................

void Downloader::chunked()
{
    std::string sLen = std::string(body);
    size_t index = sLen.find("\r\n");
    _response.byte_write -= (index  + 2);

    //Convaart string to int.
    sLen = std::string(body, 0, index);
    std::stringstream ss(sLen);
    ss >> std::hex >> _response.fileSize;
    std::cout << "File Size in Hex : " << sLen << " and in decemal : " << _response.fileSize << std::endl;
    _response.fileSize += 2;
    index += 2;

    //Problem.........................................
    if(_response.byte_write > _response.fileSize)
    {
        //Calculate how may byte need to write.....
        int extraBit = _response.byte_write - _response.fileSize; 
        _response.byte_write = _response.byte_write - extraBit;
        //Write those byte.
        _response.output.write(body + index, _response.byte_write); 
        _response.fileSize -= _response.byte_write;
        //This is remmining data.
        body += _response.byte_write + index;
        _response.byte_write = extraBit;
        chunked();
    }
    else
    {
        _response.output.write(body + index, _response.byte_write); 
        _response.fileSize -= _response.byte_write;
    }

    while(_response.fileSize > 0)
    {
        usleep(100);
        int byte_recv = SSL_read(_ssl, recvBuff, _response.fileSize);

        if(byte_recv < 1) 
           break;

        _response.fileSize -= byte_recv;
        _response.output.write(recvBuff, byte_recv); 

    }
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

