#include "socketservice.h"
#include <pthread.h>
	
SocketService::SocketService()
{
	if(pthread_attr_init(&attr) != 0){
		pritnf("Fail to pthread_attr\n");
	}
	if(pthread_create(&thr, &attr, threadImp, NULL) != 0){
		printf("Fail to pthread_create\n");
	}

	stReadFS = stWriteFS = stEceptFS = 0;
	stReadFSTmp = stWriteFSTmp = stEceptFSTmp = 0;

	if (pipe(mctrlPipe) <0){
		printf("Fial to create pipe\m");
	}
	FD_SET(mctrlPipe[0], EVENT_READ);

}
static SocketService* SocketService::getInstance()
{
	static SocketService* sockService = NULL;
	if(sockService == NULL){
		sockService = new SocketService();
	}
	return sockService;
}
SocketService::~SocketService(){}

bool SocketService::attachHandle(int handle, SockInfo* sockInfo)
{
	char ch = "1";
	
	sockInfo.push_back(sockInfo);

	FD_SET(handle, EVENT_READ);
	FD_SET(handle, EVENT_WRITE);
	FD_SET(handle, EVENT_EXCEPT);

	if (handle > maxFD){
			pritnf("handle is bigger than maxFD(%d)\n", maxFD);
			maxFD = handle;
	}
	write(mcntlPipe[1], &ch, 1);


}
void SocketService::detachHandle(int)
{

}
void SocketService::updateFD()
{

}

void SocketService::sendNotify(void* pInstance)
{
	pInstance->notify();
}
void *SocketService::threadImp(void* data)
{
	int nRes;
	int maxSignal;
	int nFD;

	for(;;){
		memcopy(stReadFSTmp,stReadFS);
		memcopy(stwriteFSTmp,stReadFS);
		memcopy(stExceptFSTmp,stReadFS);

		nRes = select(maxFD+1,&stReadFSTmp, &stWriteFSTmp, &stExceptFSTmp);
		maxSignal = nRes;

		list<sockInfo>::iterator itr;

		for(itr = sockInfo.begin(); itr!= sockInfo.end(); itr++){
			nFD = *iter->sockFd;
			if( FD_ISSET(nFD, &stReadFSTmp)){
				sendNotify(EVENT_READ);
				maxSignal--;
			}

			if( FD_ISSET(nFD, &stReadFSTmp)){
				sendNotify(EVENT_WRITE);
				maxSignal--;
			}

			if( FD_ISSET(nFD, &stReadFSTmp)){
				sendNotify(EVENT_EXCEPT);
				maxSignal--;
			}
			if (maxSignal <= 0){
				break;
			}
		}
	}

}

}
