#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"

class TCPCliSocket : public Socket
{
public:
	TCPCliSocket();
	~TCPCliSocket();
	bool Open(const char *, int);
	bool Connect();
	void Close();
	int Send(char*, int);
	int Recv(char*, int);
	void setEvent(int event)
	{
		sockService->updateEvent(socketFD, event);
	}

};
