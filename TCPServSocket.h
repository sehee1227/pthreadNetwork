#include <netinet/in.h>
#include "Socket.h"
#include "socketservice.h"

class TCPServSocket : public Socket
{
	int cliFD;
	sockaddr_in cli;
	int cliLen = sizeof(cli);
	void (*callback)(int);
    	SocketService* sockService;
public:
	TCPServSocket(){
        	sockService = SocketService::getInstance();
    	}	
	~TCPServSocket(){}
	bool open(sockaddr_in* sin);
	void Close();
	void notify(int);
	int Send(char*, int);
	int Recv(char*, int);
	bool Accept();
	void setCallback(void (*)(int));
};
