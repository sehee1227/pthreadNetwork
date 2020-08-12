#ifndef _TCPSOCKET_H
#define _TCPSOCKET_H

#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"

enum ServerType
{
    SERVER,
    CLIENT,
};

class TCPSocket : public Socket
{

public:
	TCPSocket();
	TCPSocket(int cliFd);
	~TCPSocket();
	bool Open(const char *, int, int);
	bool Connect();
	void Close();
	int Send(char*, int);
	int Recv(char*, int);
	int Accept();
};

#endif