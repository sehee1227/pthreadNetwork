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
	FD_ZERO(&stReadFS);
	FD_ZERO(&stWriteFS);
	FD_ZERO(&stExceptFS);

	FD_ZERO(&stReadFSTmp);
	FD_ZERO(&stWriteFSTmp);
	FD_ZERO(&stExceptFSTmp);

	if (pipe(mctrlPipe) <0){
		printf("Fial to create pipe\n");
	}
	FD_SET(mctrlPipe[0], &stReadFS);

//	if(pthread_attr_init(&attr) != 0){
//		pritnf("Fail to pthread_attr\n");
	if(pthread_create(&thr, NULL, threadImp, reinterpret_cast<void*>(this)) != 0){
		printf("Fail to pthread_create\n");
	}
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

	SockInfo sInfo;

	sInfo.sockFd = handle;
	sInfo.socket = socket;	
	
	sockList.push_back(sInfo);

	if (handle > maxFd){
			printf("handle(%d) is bigger than maxFD(%d)\n", handle, maxFd);
			maxFd = handle;
			return false;
	}
	// write(mctrlPipe[1], (void*)&ch, 1);
	
	return true;

}

void SocketService::detachHandle(int handle) 
{
	updateEvent(handle, 0);

	std::list<SockInfo>::iterator itr;
	for(itr = sockList.begin(); itr!= sockList.end(); itr++){
		SockInfo sockinfo = *itr;
		if(sockinfo.sockFd == handle){
			sockList.erase(itr);
		}
	}
	printf("detach handle\n");
}

void SocketService::updateEvent(int handle, int event)
{
	const char ch = 'A' ;

	FD_CLR(handle, &stReadFS);
	FD_SET(mctrlPipe[0], &stReadFS);

	if (event & READ_EVENT){
		FD_SET(handle, &stReadFS);
	printf("updata READ event:%x\n", event);
	}

	FD_CLR(handle, &stWriteFS);
	if (event & WRITE_EVENT){
		FD_SET(handle, &stWriteFS);
	printf("updata WRITE event:%x\n", event);
	}

	FD_CLR(handle, &stExceptFS);
	if (event & EXCEPT_EVENT){
		FD_SET(handle, &stExceptFS);
	printf("updata EXCEPT event:%x\n", event);
	}

	write(mctrlPipe[1], (void*)&ch, 1);
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

	printf("start Socket Service thread\n");

	for(;;){
		memcpy(&stReadFSTmp,&stReadFS, sizeof(stReadFS));
		memcpy(&stWriteFSTmp,&stWriteFS, sizeof(stWriteFS));
		memcpy(&stExceptFSTmp,&stExceptFS, sizeof(stExceptFS));

		FD_SET(mctrlPipe[0], &stReadFSTmp);
		
		nRes = select(maxFd+1,&stReadFSTmp, &stWriteFSTmp, &stExceptFSTmp, 0);
		maxSignal = nRes;

		if( FD_ISSET(mctrlPipe[0], &stReadFSTmp)){
			char ch[2] = {0,};
			read(mctrlPipe[0], ch, 1);
			printf("RX CRTL msg(%s)\n", ch);

			maxSignal--;
		}		

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
	return NULL;
}

