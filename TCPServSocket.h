#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"

class TCPServSocket : public Socket
{
	// int socketFD;
	int cliFD;
	sockaddr_in cli;
	int cliLen = sizeof(sockaddr_in);

	// SocketService* sockService;

public:
	TCPServSocket();
	~TCPServSocket();
	bool Open(const char *, int);
	void Close();
	int Send(char*, int);
	int Recv(char*, int);
	bool Accept();
	void setEvent(int event)
	{
		sockService->updateEvent(socketFD, event);
	}


};