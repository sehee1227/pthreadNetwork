#ifndef _TCPSOCKET_H
#define _TCPSOCKET_H

#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"

class TCPSocket : public Socket
{
	// int socketFD;
	int cliFD;
	sockaddr_in cli;
	int cliLen ;

	// SocketService* sockService;

public:
	TCPSocket();
	TCPSocket(int cliFd);
	~TCPSocket();
	bool Open(const char *, int);
	bool Connect();
	void Close();
	int Send(char*, int);
	int Recv(char*, int);
	int Accept();
	void setEvent(int event);
	// void setEvent(int event)
	// {
	// sockService->updateEvent(socketFD, event);

	// }

};

#endif