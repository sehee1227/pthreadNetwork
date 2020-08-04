#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "TCPServSocket.h"

bool TCPServSocket::open(sockaddr_in* sin)
{
	socketFD = socket();
	bind();

	setState(LISTEN);
	sockService->attachHandle(socketFD, this);
	sockService->updateEvent(READ_EVENT | EXCEPT_EVENT);
}

bool TCPServSocket::Accept()
{
	cliFD = accept();
	setState(ESTABLISHED);

}

int TCPServSocket::Send(char* pBuf, int len)
{
	int nsentByte = send(cliFD, (void*)pBuf, len, 0);

	return nsentByte;

}

int TCPServSocket::Recv(char* pBuf, int len)
{
	int nrecvByte = recv(cliFD, (void*)pBuf, len, 0);

	return nrecvByte;

}

void TCPServSocket::notify(int socketEvent)
{
	printf("Send notify\n");
	if (callback != NULL){
		callback(socketEvent);
	}
	return;
}

void TCPServSocket::setCallback(void (*cbFunc)(int)){
	callback = cbFunc;
	return;
}
