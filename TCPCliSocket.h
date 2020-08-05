#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"
#include "socketservice.h"

class TCPCliSocket : public Socket
{
    SocketService* sockService;
public:
	TCPCliSocket();
	~TCPCliSocket();
	bool Open(const char *, int);
	void Close();
	int Send(char*, int);
	int Recv(char*, int);

};
