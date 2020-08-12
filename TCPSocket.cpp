#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "TCPSocket.h"

TCPSocket::TCPSocket()
{
       printf("TCPSocket()\n");
}

TCPSocket::TCPSocket(int cliFd)
{
   	sockService = SocketService::getInstance();
	socketFD = cliFd;
	sockService->attachHandle(socketFD, this);
	setState(ESTABLISHED);
}

TCPSocket::~TCPSocket()
{
   	printf("~TCPSocket :%d \n", socketFD);
    // delete sockService;
}	

bool TCPSocket::Open(const char* addr, int port, int type)
{
	struct sockaddr_in sin;

    printf("addr: %s\n", addr);

    memset(&sin, 0x00, sizeof(struct sockaddr_in));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    
    if (type == SERVER){
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        sin.sin_addr.s_addr = inet_addr(addr);
    }

	socketFD = socket(AF_INET, SOCK_STREAM, 0);

    if (type == SERVER){

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

        if (listen(socketFD, 10) <0){
            printf("Fail to listen socketFD: %d\n", socketFD);
            fprintf(stderr, "listen error: %s\n", strerror(errno));
            return false;
        }

        setState(LISTEN);

        usleep(1000);

        sockService->attachHandle(socketFD, this);
        sockService->updateEvent(socketFD, (READ_EVENT ));

        printf("TCPServSocket Open\n");

        return true;
    } else {

        usleep(1000);

        int flag = fcntl(socketFD, F_GETFL, 0);
        fcntl(socketFD, F_SETFL, flag | O_NONBLOCK);

        if (connect(socketFD, (struct sockaddr*)&sin, sizeof(sin)) < 0){
            printf("Fail to Connect socketFD: %d\n", socketFD);
            fprintf(stderr, "Connect error: %s\n", strerror(errno));
            if (errno ==  EINPROGRESS){
                sockService->attachHandle(socketFD, this);
                sockService->updateEvent(socketFD, WRITE_EVENT);
                setState(CONNECT);
                return true;
            }
            else false;
        }
    }

    return true;
}

bool TCPSocket::Connect()
{
	int error = 0;
	socklen_t len = sizeof(error);

    printf("TCPClivSocket CONNECT");

	if( getsockopt(socketFD, SOL_SOCKET, SO_ERROR, (void*)&error, &len) < 0){
		printf("Fail to connect wait socketFD: %d\n", socketFD);
	    fprintf(stderr, "connect wait error: %s\n", strerror(errno));
		return false;
	}

	sockService->updateEvent(socketFD, (READ_EVENT));
	setState(ESTABLISHED);

	printf("TCPClivSocket ESTABLISHED\n");
	
	return true;
}

int TCPSocket::Accept()
{
	int cliFD, cliLen;
	sockaddr_in cli;

	cliLen = sizeof(cli);
	cliFD = accept(socketFD, (struct sockaddr*)&cli, (socklen_t*)&cliLen);
	if(cliFD < 0){
		printf("Fail to accept: %d\n", socketFD);
		fprintf(stderr, "accept error: %s\n", strerror(errno));
		return false;
	}

	setState(ESTABLISHED);

	int flag = fcntl(cliFD, F_GETFL, 0);
	fcntl(cliFD, F_SETFL, flag | O_NONBLOCK);

	return cliFD;
}

int TCPSocket::Send(char* pBuf, int len)
{
	int nsentByte = send(socketFD, (void*)pBuf, len+1, 0);
	if (nsentByte < 0){
	   fprintf(stderr, "send error: %s\n", strerror(errno));
	}
	return nsentByte;

}

int TCPSocket::Recv(char* pBuf, int len)
{
	int nrecvByte = recv(socketFD, (void*)pBuf, len, 0);
	if (nrecvByte < 0){
	//    fprintf(stderr, "recv error: %s\n", strerror(errno));
	}
	return nrecvByte;
}

void TCPSocket::Close()
{
	sockService->detachHandle(socketFD);
	close(socketFD);

}
