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
   	printf("~TCPServSocket\n");
}	

bool TCPCliSocket::Open(const char* addr, int port)
{
	struct sockaddr_in sin;
    pthread_t thr;

    printf("addr: %s\n", addr);

    memset(&sin, 0x00, sizeof(struct sockaddr_in));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(addr);

	socketFD = socket(AF_INET, SOCK_STREAM, 0);

	int flag;
	flag = fcntl(socketFD, F_GETFL, 0);
	fcntl(socketFD, F_SETFL, flag | O_NONBLOCK);

	if(connect(socketFD, (struct sockaddr*)&sin, sizeof(sin))== -1){
		printf("Fail to connect socketFD: %d\n", socketFD);
	    fprintf(stderr, "connect error: %s\n", strerror(errno));
		return -1;
	}

	setState(ESTABLISHED);
	
	sockService->attachHandle(socketFD, this);
	sockService->updateEvent(socketFD, READ_EVENT | WRITE_EVENT | EXCEPT_EVENT);

	printf("TCPClivSocket Open\n");
	
	return true;
}



int TCPCliSocket::Send(char* pBuf, int len)
{
	int nsentByte = send(socketFD, (void*)pBuf, len, 0);

	return nsentByte;

}

int TCPCliSocket::Recv(char* pBuf, int len)
{
	int nrecvByte;
       if (nrecvByte = recv(socketFD, (void*)pBuf, len, 0) == -1){
	       fprintf(stderr, "recv error: %s\n", strerror(errno));
       }
	return nrecvByte;

}

void TCPCliSocket::Close()
{
	sockService->detachHandle(socketFD);
	close(socketFD);

}
