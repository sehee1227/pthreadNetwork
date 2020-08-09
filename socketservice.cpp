#include "socketservice.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <cstring>
#include <list>
#include <fcntl.h>
#include <errno.h>


void *SocketService::threadImp(void* param)
{
	
	SocketService* instance = static_cast<SocketService*>(param);
	return instance->run();	
}

SocketService::SocketService()
{
	threadRef = 0;
	stopThread = false;

	FD_ZERO(&stReadFS);
	FD_ZERO(&stWriteFS);
	FD_ZERO(&stExceptFS);

	FD_ZERO(&stReadFSTmp);
	FD_ZERO(&stWriteFSTmp);
	FD_ZERO(&stExceptFSTmp);
	
	if (pipe(mctrlPipe) < 0){
		printf("fail to open Pipe\n");
	}
	if (mctrlPipe[0] != -1){
		int flag = fcntl(mctrlPipe[0], F_GETFL, 0);
		flag |= O_NONBLOCK;
		fcntl(mctrlPipe[0], F_SETFL, flag);
	}
	if (mctrlPipe[1] != -1){
		int flag = fcntl(mctrlPipe[1], F_GETFL, 0);
		flag |= O_NONBLOCK;
		fcntl(mctrlPipe[1], F_SETFL, flag);
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
SocketService::~SocketService(){}

bool SocketService::attachHandle(int handle, Socket* socket)
{

	SockInfo sInfo;

	sInfo.sockFd = handle;
	sInfo.socket = socket;	
	
	sockList.push_back(sInfo);

	if (handle > maxFd){
			// printf("handle(%d) is bigger than maxFD(%d)\n", handle, maxFd);
			maxFd = handle;
	}
	threadRef++;
	return true;

}

void SocketService::detachHandle(int handle) 
{

	updateEvent(handle, 0);

	std::list<SockInfo>::iterator itr;

	for(itr = sockList.begin(); itr!= sockList.end(); itr++){
		SockInfo sockinfo = *itr;
		if(sockinfo.sockFd == handle){
			sockList.erase(itr++);
		}
	}
	threadRef--;
	if (threadRef == 0){
		terminateThread();
	}
}

void SocketService::terminateThread()
{
	stopThread = true;

	int res;
	char ch[4] = "AA" ;
	res = write(mctrlPipe[1], &ch, 2);
	if (res <0){
		printf("Pipe write fail on mctrlPipe[1]:%d\n",res);
		fprintf(stderr, "recv error: %s\n", strerror(errno));
	}


}
void SocketService::updateEvent(int handle, int event)
{

	printf ("Update event Handle: %d, event: %d\n", handle, event);
	char ch[4] = "AA" ;

	FD_CLR(handle, &stReadFS);
	FD_SET(mctrlPipe[0], &stReadFS);

	if (event & READ_EVENT){
		FD_SET(handle, &stReadFS);
	}

	FD_CLR(handle, &stWriteFS);
	if (event & WRITE_EVENT){
		FD_SET(handle, &stWriteFS);
	}

	FD_CLR(handle, &stExceptFS);
	if (event & EXCEPT_EVENT){
		FD_SET(handle, &stExceptFS);
	}

	int res;
	res = write(mctrlPipe[1], &ch, 2);
	if (res <0){
		fprintf(stderr, "recv error: %s\n", strerror(errno));
	}
	sockCond.signal();

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
	// struct timeval tv;
	// tv.tv_sec = 1;
	// tv.tv_usec = 0;
	
	printf("start Socket Service thread\n");
	sockCond.wait();
	printf("start Socket Service thread1\n");

	for(;;){
		if(stopThread){
			printf("terminate SocketService Thread\n");
			break;
		}
		memcpy(&stReadFSTmp,&stReadFS, sizeof(stReadFS));
		memcpy(&stWriteFSTmp,&stWriteFS, sizeof(stWriteFS));
		memcpy(&stExceptFSTmp,&stExceptFS, sizeof(stExceptFS));
		
		FD_SET(mctrlPipe[0], &stReadFSTmp);

		nRes = select(maxFd+1,&stReadFSTmp, &stWriteFSTmp, &stExceptFSTmp, 0);
		if (nRes <0){
			printf("Fail to select\n");
	    	fprintf(stderr, "select  error: %s\n", strerror(errno));
			return NULL;
		}
		maxSignal = nRes;

		if( FD_ISSET(mctrlPipe[0], &stReadFSTmp)){
			char ch[4] = {0,};
			read(mctrlPipe[0], ch, 2);
			maxSignal--;
			if (maxSignal <= 0){
				continue;
			}
		}

		std::list<SockInfo>::iterator itr;
		
		for(itr = sockList.begin(); itr!= sockList.end(); itr++){
			SockInfo sockinfo = *itr;
			nFD = sockinfo.sockFd;
			if( FD_ISSET(nFD, &stReadFSTmp)){
				sendNotify(sockinfo.socket, READ_EVENT);
				maxSignal--;
			}

			if( FD_ISSET(nFD, &stWriteFSTmp)){
				sendNotify(sockinfo.socket, WRITE_EVENT);
				maxSignal--;
			}

			if( FD_ISSET(nFD, &stExceptFSTmp)){
				sendNotify(sockinfo.socket, EXCEPT_EVENT);
				maxSignal--;
			}
			if (maxSignal <= 0){
				break;
			}
		}
	}
	pthread_exit(0);
	return NULL;
}

