#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Socket.h"
#include "socketservice.h"

class TCPServSocket : public Socket
{
	// int socketFD;
	int cliFD;
	sockaddr_in cli;
	int cliLen = sizeof(cli);
	
    	SocketService* sockService;
public:
	TCPServSocket();
	~TCPServSocket();
	bool Open(sockaddr_in* sin);
	void Close();
	int Send(char*, int);
	int Recv(char*, int);
	bool Accept();

};
// TCPServSocket::TCPServSocket()
// {
//    	sockService = SocketService::getInstance();
// }

// TCPServSocket::~TCPServSocket()
// {
//    	printf("~TCPServSocket\n");
// }	

// bool TCPServSocket::Open(sockaddr_in* sin)
// {
	
// 	socketFD = socket(AF_INET, SOCK_STREAM, 0);
// 	bind(socketFD, (struct sockaddr*)&sin, sizeof(sin));

// 	setState(LISTEN);
	
// 	sockService->attachHandle(socketFD, this);
// 	sockService->updateEvent(socketFD, READ_EVENT | EXCEPT_EVENT);
	
// 	return true;
// }

// bool TCPServSocket::Accept()
// {
// 	cliFD = accept(socketFD, (struct sockaddr*)&cli, (socklen_t*)&cliLen );
// 	setState(ESTABLISHED);
// 	return true;

// }

// int TCPServSocket::Send(char* pBuf, int len)
// {
// 	int nsentByte = send(cliFD, (void*)pBuf, len, 0);

// 	return nsentByte;

// }

// int TCPServSocket::Recv(char* pBuf, int len)
// {
// 	int nrecvByte = recv(cliFD, (void*)pBuf, len, 0);

// 	return nrecvByte;

// }

// void TCPServSocket::Close()
// {
// 	sockService->detachHandle(socketFD);
// 	close(cliFD);
// 	close(socketFD);

// }