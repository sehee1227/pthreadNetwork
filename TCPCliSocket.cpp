#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "TCPCliSocket.h"


TCPCliSocket::TCPCliSocket()
{
   	sockService = SocketService::getInstance();
}

TCPCliSocket::~TCPCliSocket()
{
	close(socketFD);
   	printf("~TCPServSocket\n");
}	

bool TCPCliSocket::Open(const char* addr, int port)
{
	struct sockaddr_in sin;

    printf("addr: %s\n", addr);

    memset(&sin, 0x00, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(addr);

	socketFD = socket(AF_INET, SOCK_STREAM, 0);

	if(connect(socketFD, (struct sockaddr*)&sin, sizeof(sin))== -1){
		printf("Fail to connect socketFD: %d\n", socketFD);
	    fprintf(stderr, "connect error: %s\n", strerror(errno));
		if (errno == EINPROGRESS){
			sockService->updateEvent(socketFD, WRITE_EVENT | EXCEPT_EVENT);
			setState(CONNECT);
			return true;
		}
		return false;
	}

	int flag = fcntl(socketFD, F_GETFL, 0);
	fcntl(socketFD, F_SETFL, flag | O_NONBLOCK);
	setState(ESTABLISHED);
	
	sockService->attachHandle(socketFD, this);
	sockService->updateEvent(socketFD, READ_EVENT | EXCEPT_EVENT);

	printf("TCPClivSocket ESTABLISHED\n");
	
	return true;
}


bool TCPCliSocket::Connect()
{
	int error = 0;
	socklen_t len = sizeof(error);

    printf("TCPClivSocket CONNECT");

	if( getsockopt(socketFD, SOL_SOCKET, SO_ERROR, (void*)&error, &len) < 0){
		printf("Fail to connect wait socketFD: %d\n", socketFD);
	    fprintf(stderr, "connect wait error: %s\n", strerror(errno));
	    	sockService->updateEvent(socketFD, EXCEPT_EVENT);
		return false;
	}

	int flag = fcntl(socketFD, F_GETFL, 0);
	fcntl(socketFD, F_SETFL, flag | O_NONBLOCK);

	setState(ESTABLISHED);
	
	sockService->attachHandle(socketFD, this);
	sockService->updateEvent(socketFD, (READ_EVENT | EXCEPT_EVENT));

	printf("TCPClivSocket ESTABLISHED\n");
	
	return true;
}

int TCPCliSocket::Send(char* pBuf, int len)
{
	int nsentByte = send(socketFD, (void*)pBuf, len, 0);
	// if (nsentByte < 0){
		fprintf(stderr, "send error: %s\n", strerror(errno));
	// }
	return nsentByte;

}

int TCPCliSocket::Recv(char* pBuf, int len)
{
	int nrecvByte= recv(socketFD, (void*)pBuf, len, 0);
       if (nrecvByte < 0){
	       fprintf(stderr, "recv error: %s\n", strerror(errno));
       }
	return nrecvByte;

}

void TCPCliSocket::Close()
{
	sockService->updateEvent(socketFD, 0);
	sockService->detachHandle(socketFD);
	close(socketFD);

}

