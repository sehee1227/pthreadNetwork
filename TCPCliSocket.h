#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"

class TCPCliSocket : public Socket
{
	int socketFD;
	SocketService* sockService;
public:
	TCPCliSocket();
	~TCPCliSocket();
	bool Open(const char *, int);
	bool Connect();
	void Close();
	int Send(char*, int);
	int Recv(char*, int);

};
