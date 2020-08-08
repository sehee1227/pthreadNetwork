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
	close(cliFD);
	close(socketFD);
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

	int nSockOpt = 1;
	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt)) < 0) {
		printf("Fail to setsockopt socketFD: %d\n", socketFD);
	    fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
		return false;
	}
	if(bind(socketFD, (struct sockaddr*)&sin, sizeof(sin)) < 0){
		printf("Fail to bind socketFD: %d\n", socketFD);
	    fprintf(stderr, "bind error: %s\n", strerror(errno));
		return false;
	}

	int flag = fcntl(socketFD, F_GETFL, 0);
	fcntl(socketFD, F_SETFL, flag | O_NONBLOCK);

	if (listen(socketFD, 5) <0){
		printf("Fail to listen socketFD: %d\n", socketFD);
	    fprintf(stderr, "listen error: %s\n", strerror(errno));
		return false;
	}

	setState(LISTEN);

	sockService->attachHandle(socketFD, this);
	sockService->updateEvent(socketFD, (READ_EVENT | EXCEPT_EVENT));

	printf("TCPServSocket Open\n");
	
	return true;
}

bool TCPServSocket::Accept()
{
	cliFD = accept(socketFD, (struct sockaddr*)&cli, (socklen_t*)&cliLen);
	if(cliFD < 0){
		printf("Fail to accept: %d\n", socketFD);
		fprintf(stderr, "accept error: %s\n", strerror(errno));
		return false;
	}

	// int flag = fcntl(socketFD, F_GETFL, 0);
	// fcntl(socketFD, F_SETFL, flag | O_NONBLOCK);
	setState(ESTABLISHED);

	sockService->updateEvent(socketFD, (READ_EVENT | EXCEPT_EVENT));
	return true;

}

int TCPServSocket::Send(char* pBuf, int len)
{
	int nsentByte = send(cliFD, (void*)pBuf, len+1, MSG_DONTWAIT);

	return nsentByte;

}

int TCPServSocket::Recv(char* pBuf, int len)
{
	int nrecvByte = recv(cliFD, (void*)pBuf, len, MSG_DONTWAIT);
	if (nrecvByte < 0){
	   fprintf(stderr, "recv error: %s\n", strerror(errno));
	}
	return nrecvByte;
}

void TCPServSocket::Close()
{
	sockService->updateEvent(socketFD, 0);
	sockService->detachHandle(socketFD);
	close(cliFD);
	close(socketFD);

}
