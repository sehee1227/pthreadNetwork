#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "TCPServSocket.h"

TCPServSocket::TCPServSocket()
{
   	sockService = SocketService::getInstance();
}

TCPServSocket::~TCPServSocket()
{
   	printf("~TCPServSocket\n");
}	

bool TCPServSocket::Open(sockaddr_in* sin)
{
	
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	bind(socketFD, (struct sockaddr*)&sin, sizeof(sin));

	setState(LISTEN);
	
	sockService->attachHandle(socketFD, this);
	sockService->updateEvent(socketFD, READ_EVENT | EXCEPT_EVENT);

	printf("TCPServSocket Open\n");
	
	return true;
}

bool TCPServSocket::Accept()
{
	cliFD = accept(socketFD, (struct sockaddr*)&cli, (socklen_t*)&cliLen );
	setState(ESTABLISHED);
	return true;

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

void TCPServSocket::Close()
{
	sockService->detachHandle(socketFD);
	close(cliFD);
	close(socketFD);

}
// void TCPServSocket::notify(int socketEvent)
// {
// 	printf("Send notify\n");
// 	if (callback != NULL){
// 		callback(socketEvent);
// 	}
// 	return;
// }

// void TCPServSocket::setCallback(void (*cbFunc)(int)){
// 	callback = cbFunc;
// 	return;
// }