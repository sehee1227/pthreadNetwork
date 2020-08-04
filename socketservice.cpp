#include "socketservice.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <cstring>
#include <list>

//using namespace std;

void *SocketService::threadImp(void* param)
{
	
	SocketService* instance = static_cast<SocketService*>(param);
	return instance->run();	
}

SocketService::SocketService()
{
//	if(pthread_attr_init(&attr) != 0){
//		pritnf("Fail to pthread_attr\n");
//	}
	if(pthread_create(&thr, NULL, threadImp, reinterpret_cast<void*>(this)) != 0){
		printf("Fail to pthread_create\n");
	}

	//stReadFS = stWriteFS = stEceptFS = 0;
	//stReadFSTmp = stWriteFSTmp = stEceptFSTmp = 0;

	if (pipe(mctrlPipe) <0){
		printf("Fial to create pipe\n");
	}
	//FD_SET(mctrlPipe[0], READ_EVENT);

}

SocketService* SocketService::getInstance()
{
	static SocketService* sockService = NULL;
	if(sockService == NULL){
		sockService = new SocketService();
	}
	return sockService;
}
//SocketService::~SocketService(){}

bool SocketService::attachHandle(int handle, Socket* socket)
{
	const char ch = 'A' ;
	SockInfo sInfo;

	sInfo.sockFd = handle;
	sInfo.socket = socket;	
	
	sockList.push_back(sInfo);

	//FD_SET(handle, READ_EVENT);
	//FD_SET(handle, WRITE_EVENT);
	//FD_SET(handle, EXCEPT_EVENT);

	if (handle > maxFd){
			printf("handle is bigger than maxFD(%d)\n", maxFd);
			maxFd = handle;
			return false;
	}
	write(mctrlPipe[1], (void*)&ch, 1);
	
	return true;

}

void SocketService::detachHandle(int handle)
{
	printf("detach handle\n");
}

void SocketService::updateEvent(int event)
{

	printf("updata event\n");
}

void SocketService::sendNotify(Socket* pInstance, int event)
{
	pInstance->notify(event);
}

void* SocketService::run(void)
{
	int nRes;
	int maxSignal;
	int nFD;

	for(;;){
		memcpy(&stReadFSTmp,&stReadFS, sizeof(stReadFS));
		memcpy(&stWriteFSTmp,&stWriteFS, sizeof(stWriteFS));
		memcpy(&stExceptFSTmp,&stExceptFS, sizeof(stExceptFS));

		nRes = select(maxFd+1,&stReadFSTmp, &stWriteFSTmp, &stExceptFSTmp, 0);
		maxSignal = nRes;

		std::list<SockInfo>::iterator itr;
		
		for(itr = sockList.begin(); itr!= sockList.end(); itr++){
			SockInfo sockinfo = *itr;
			nFD = sockinfo.sockFd;
			if( FD_ISSET(nFD, &stReadFSTmp)){
				sendNotify(sockinfo.socket, READ_EVENT);
				maxSignal--;
			}

			if( FD_ISSET(nFD, &stReadFSTmp)){
				sendNotify(sockinfo.socket, WRITE_EVENT);
				maxSignal--;
			}

			if( FD_ISSET(nFD, &stReadFSTmp)){
				sendNotify(sockinfo.socket, EXCEPT_EVENT);
				maxSignal--;
			}
			if (maxSignal <= 0){
				break;
			}
		}
	}

}
	return NULL;
}
