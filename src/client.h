#ifndef CLIENT
#define CLIENT

#include "header.h"
#include "structure.h"

#define BUFFER_SIZE 32768

class Client 
{
    public:
        Client();
            Response send_head_req(URL &url);
        ~Client();

	protected:
		char sendBuff[2048];
		char recvBuff[32768];
		long kb = 1024;
		long mb = 1048576;
		long gb = 1073741824;

        int sock;
        SSL_CTX *_ctx;
        SSL *_ssl;
        struct addrinfo hints;
        struct addrinfo *bind_addr;

        //Function ...................
        bool connectServer(std::string ip, std::string port, std::string protocol);
        int recv_response(std::string protocol, char* ptr, int len);
        bool send_get_req(URL &url, long byteRange);
        void clientClose();
        float getSize(long number);
};


#endif
