#include "client.h"

Client::Client()
{
    bzero(&hints, sizeof(hints));
}

bool Client::connectServer(std::string ip, std::string port, std::string protocol)
{
	_protocol = protocol;
    //This is for Broke Pipe exception handler for sending to a client which is close.
	signal(SIGPIPE, SIG_IGN);    

    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo(ip.c_str(), port.c_str(), &hints, &bind_addr))
    {
        perror("Faield to getaddrinfo()");
        return false;
    }

	sock = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
	if(sock < 0)
    {
		std::cout << "Error to open socket ..!" << std::endl;
        return false;
    }

    //Connecting to server......
    if(connect(sock, bind_addr->ai_addr, bind_addr->ai_addrlen) != 0)
    {
        std::cout << "Error to connect server ..!" << std::endl;
        return false;
    }
    freeaddrinfo(bind_addr); 

    std::cout << "Connected" << std::endl;
    return true;
}

bool Client::send_req(std::string hostname, std::string port, std::string path)
{
	bzero(sendBuff, 2048);
    sprintf(sendBuff, "GET /%s HTTP/1.1\r\n", path.c_str());
    sprintf(sendBuff + strlen(sendBuff), "Host: %s:%s\r\n", hostname.c_str(), port.c_str());
    sprintf(sendBuff + strlen(sendBuff), "Connection: close\r\n");   
    sprintf(sendBuff + strlen(sendBuff), "User-Agent: honpwc web_get 1.0\r\n");
    sprintf(sendBuff + strlen(sendBuff), "\r\n");
	
	//Send.........................
	if(_protocol == "http")
		send(sock, sendBuff , strlen(sendBuff), 0);

	return true;
}


float Client::getSize(long number)
{
	if(number > gb)
		 return (float)number / gb ;
	else if(number > mb)
		return (float)number / mb;
	else
		return (float)number / kb;
}

 
    
void Client::clientClose()
{
    close(sock);
}

Client::~Client()
{
    if(sock)
        close(sock);

}
 
