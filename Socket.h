#ifndef SOCKET_H
#define SOCKET_H

#include "socketservice.h"

class SocketService;

enum socketState {
		CLOSED,
		LISTEN,
		CONNECT,
		ESTABLISHED,
		CLOSING,
};
class Socket
{
	int 	state;
	void (*callback)(int);

protected:
		int socketFD;
		int eventState;
		SocketService* sockService;
public:
	Socket();
	virtual ~Socket();
	virtual bool Open(const char *, int, int) = 0;
	virtual	bool Connect() = 0;
	virtual void Close() = 0;
	virtual int Send(char*, int) = 0;
	virtual int Recv(char*, int) = 0;
	virtual int Accept() = 0;
	void setState(int state);
	int getState();
	void notify(int socketEvent);
	void setCallback(void (*cbFunc)(int));
	void setEvent(int event);
};

#endif 
