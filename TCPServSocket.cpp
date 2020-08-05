#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "TCPServSocket.h"

TCPServSocket::TCPServSocket()
{
   	sockService = SocketService::getInstance();
}

TCPServSocket::~TCPServSocket()
{
   	printf("~TCPServSocket\n");
}	

bool TCPServSocket::Open(const char* addr, int port)
{
	struct sockaddr_in sin;

    printf("addr: %s\n", addr);

    memset(&sin, 0x00, sizeof(struct sockaddr_in));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    //sin.sin_addr.s_addr = inet_addr(addr);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
	
	socketFD = socket(AF_INET, SOCK_STREAM, 0);

	int flag;
	flag = fcntl(socketFD, F_GETFL, 0);
	fcntl(socketFD, F_SETFL, flag | O_NONBLOCK);

	int nSockOpt = 1;
	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));
	if(bind(socketFD, (struct sockaddr*)&sin, sizeof(sin))== -1){
		printf("Fail to bind socketFD: %d\n", socketFD);
	    fprintf(stderr, "bind error: %s\n", strerror(errno));
		return false;
	}

	if (listen(socketFD, 5) <0){
		printf("Fail to listen socketFD: %d\n", socketFD);
	    fprintf(stderr, "listen error: %s\n", strerror(errno));
		return false;
	}

	setState(LISTEN);
	
	sockService->attachHandle(socketFD, this);
	sockService->updateEvent(socketFD, READ_EVENT | EXCEPT_EVENT);

	printf("TCPServSocket Open\n");
	
	return true;
}

bool TCPServSocket::Accept()
{
	cliFD = accept(socketFD, (struct sockaddr*)&cli, (socklen_t*)&cliLen );
	if(cliFD == -1){
		printf("Fail to accept: %d\n", socketFD);
		fprintf(stderr, "accept error: %s\n", strerror(errno));
		return false;
	}
	setState(ESTABLISHED);
	sockService->updateEvent(socketFD, READ_EVENT | WRITE_EVENT | EXCEPT_EVENT);
	return true;

}

int TCPServSocket::Send(char* pBuf, int len)
{
	int nsentByte = send(cliFD, (void*)pBuf, len, 0);

	return nsentByte;

}

int TCPServSocket::Recv(char* pBuf, int len)
{
	int nrecvByte;
       if (nrecvByte = recv(cliFD, (void*)pBuf, len, 0) == -1){
	       fprintf(stderr, "recv error: %s\n", strerror(errno));
       }
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
