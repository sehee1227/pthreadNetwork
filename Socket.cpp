#include "Socket.h"
#include <arpa/inet.h>
#include <unistd.h>


#include <stdio.h>

Socket::Socket()
{
 	sockService = SocketService::getInstance();
   	printf("Socket() & SocketService::getInstance() \n");
}

Socket::~Socket()
{
	if (!sockService->isRunnung()){
    	delete sockService;
	   	printf("Delete sockService in ~Socket():%d \n", socketFD);
	}

   	printf("~Socket() :%d \n", socketFD);
}

void Socket::setState(int state)
{
	this->state = state;
}

int Socket::getState()
{
	return this->state;
}

void Socket::notify(int socketEvent)
{
	if (callback != NULL){
		callback(socketEvent);
	}
	return;
}

void Socket::setCallback(void (*cbFunc)(int))
{
	callback = cbFunc;
	return;
}

void Socket::setEvent(int event)
{
	sockService->updateEvent(socketFD, event);
}

