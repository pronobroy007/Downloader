#ifndef CLIENT
#define CLIENT

#include "header.h"

#define BUFFER_SIZE 32768

class Client 
{
    public:
        Client();
            bool connectServer(std::string ip, std::string port, std::string protocol);
			bool send_req(std::string hostname, std::string port, std::string path);
            void clientClose();
			float getSize(long number);
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

	private:
		std::string _protocol;

};


#endif
