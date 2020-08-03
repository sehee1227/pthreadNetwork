#include "TCPServSocket.h"

bool TCPServSocket::open(sockaddr_in* sin)
{
	
	socketFD = socket();
	bind();

	setState(LISTEN);
	
	sockService->attachHandle(socketFD, this);
	sockService->updateEvent(READ_EVNET | EXCEPT_EVENT);
}

bool TCPServSocket::accept()
{
	cliFD = accept();
	setState(ESTABLISHED);

}

int TCPServSocket::send(char* pBuf, int len)
{
	int nsentByte = send(cliFD, (void*)pBuf, len);

	return nsentByte;

}

int TCPServSocket::recv(char* pBuf)
{
	int nrecvByte = recv(cliFD, (void*)pBuf, len, 0);

	return nrecvByte;

}

void TCPServSocket::notifyEv(socketEvent)
{
	pritnf("Send notify\n");
	if (callback != NULL){
		callback(socketEvent);
	}
	return;
}

void TCPServSocket::setCallback(cbFunc){
	callback = cbFunc;
	return;
}