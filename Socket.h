#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>

enum socketState : int{
		CLOSED,
		LISTEN,
		ESTABLISHED,
		CLOSING,
};
class Socket
{



	int 	state;
	void (*callback)(int);

	fd_set 	readFd;
	fd_set 	writeFd;
	fd_set	exceptFd;

	fd_set 	readFdTmp;
	fd_set 	writeFdTmp;
	fd_set	exceptFdTmp;
protected:
		int socketFD;
public:
	void setState(int state)
	{
		this->state = state;
	}
	int getState()
	{
		return this->state;
	}
	void notify(int socketEvent)
	{
		printf("Socket.h Send notify handle: %d, event:%d\n",socketFD, socketEvent);
		if (callback != NULL){
			callback(socketEvent);
		}
		return;
	}
	void setCallback(void (*cbFunc)(int))
	{
		callback = cbFunc;
		return;
	}
};

#endif 
