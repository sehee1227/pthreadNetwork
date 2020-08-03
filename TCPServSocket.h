#include <netinet/in.h>
#include "Socket.h"
#include "socketservice.h"

class TCPServSocket : public Socket
{
	int cliFD;
	sockaddr_in cli;
	int cliLen = sizeof(cli);
	void (*callback)(int);
    SockService* sockService;
public:
	TCPServSocket(
        sockService = SocketService::getInstance();
    );
	~TCPServSocket();
	virtual bool open(sockaddr_in* sin);
	virtual void close();
	virtual void notifyEv(int);
	int send(char*, int);
	int recv(char*);
	bool accept();
	void setCallback();
};
