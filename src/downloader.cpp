#include "downloader.h"

Downloader::Downloader()
{
	_response.encoding = FILE_ENCODING::def;
    projectDir = "/home/linux/Documents/program/app/downloader/";
}

void Downloader::init(std::string address, URL &url)
{
	//Parser url...........
    url.url = address;
    Parser::parse_url(url);
	if(url.fileName.empty())
		url.fileName = "index.html";
}

bool Downloader::download(URL &url, long byteRange)
{
	//Connecting to server........................
	connectServer(url.hostname, url.port, url.protocol);
    send_get_req(url, byteRange);
    
	//Downloading..............................
    int p = 0;
    char *ptr = (char*)recvBuff;
    _response.fileName = url.fileName;

    while (true)
    {
		int byte_recv = recv_response(url.protocol, ptr + p, BUFFER_SIZE - p);
        if(byte_recv < 1) 
        {
            cleanUp(); 
            return false;
        }
	
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
                _response.header = std::string(recvBuff);
                ssize_t index = _response.header.find("\r\n\r\n");
                _response.header = std::string(recvBuff, 0, index);
				/******************************************************
                // This function set "fileName" if exist, 
                // "enccoding" type. also set "byte_write" 
                // and "Content_Type"
				******************************************************/
                Parser::parseHeader(_response);
				_response.fileSizeShow = getSize(_response.fileSize);

                if(!url.resumeDownload && _response.encoding == FILE_ENCODING::length)
                {
                    db.init();
                    std::vector<std::string> buff;
                    buff.push_back(url.url);
                    buff.push_back(std::to_string(_response.fileSize));
                    buff.push_back(_response.fileName);
                    buff.push_back("Processing");
                    std::string sql = "INSERT INTO Tmp(Url, FileSize, FileName, Status) VALUES(?, ?, ?, ?)";
                    db.prepareStmt(buff,  sql, 0);

                    buff.clear();
                    sql = "SELECT last_insert_rowid()";
                    db.prepareStmt(buff,  sql, 1);
                    url.downloadID = db.data[0][0];
                    db.close();
                }
				/******************************************************/
				// This many data need to write first time.
				// Because data and header mixed together.
				// so get read of header part form actual data.
				/******************************************************/
				_response.byte_write = p - (_response.header.size() + 4); 
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
                        _response.output.open(_response.fileName, std::ios::binary | std::ios::app);
                    
                    //Download when file is given as "Content_Length"
                    content_length();
                    
                    //Make sure program end after download.
                    if(_response.totalDownSize >= _response.fileSize)
                    {
                        //update Database..............
                        db.init();
                        std::vector<std::string> buff;
                        buff.push_back("Done");
                        buff.push_back(url.downloadID);
                        std::string sql = "UPDATE Tmp SET status=? WHERE User_ID=?"; 
                        db.prepareStmt(buff, sql, 0);
                        db.close();

                        cleanUp();
                        return true;
                    }
                    break;
                }

                case FILE_ENCODING::chunked:
                {
                    if(!_response.output.is_open())
                        _response.output.open(_response.fileName);
                    
                    //Download when file is given as "Chunked"
                    if(_response.byte_write > 0)
                        if(!chunked(url.protocol))
                            return false;
                    break;	
                }

                case FILE_ENCODING::connection:
                {
                    if(!_response.output.is_open())
                        _response.output.open(_response.fileName);
                        
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

bool Downloader::chunked(std::string protocol)
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
        chunked(protocol);
    }
    else
    {
        _response.output.write(body + index, _response.byte_write); 
        _response.fileSize -= _response.byte_write;
    }

    while(_response.fileSize > 0)
    {
		int byte_recv = recv_response(protocol, recvBuff, _response.fileSize);
        if(byte_recv < 1) 
        {
            cleanUp(); 
            return false;
        }

        _response.fileSize -= byte_recv;
        _response.output.write(recvBuff, byte_recv); 
    }

    return true;
}

//******************************************************************************************
void Downloader::cleanUp()
{
	body = 0;
	_response.clear();
    db.close();
	std::cout << "Cleaning up ............." << std::endl;
}


Downloader::~Downloader()
{
	body = 0;
	_response.clear();
    db.close();
}



