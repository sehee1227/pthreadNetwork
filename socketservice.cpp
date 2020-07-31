#include "socketservice.h"

	struct sockInfo{
		Socket* plistener; 	
	}
	map<int, sockInfo*> sockHandle;
	
	int mctrlPipe[2];
	int	mcntlEvent;

	fd_set	stReadFS;
	fd_set	stWriteFS;
	fd_set	stEceptFS;

	fd_set	stReadFSTmp;
	fd_set	stWriteFSTmp;
	fd_set	stEceptFSTmp;
	
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
static SocketService*SocketService::getInstance()
{
	static SocketService* sockService = NULL;
	if(sockService == NULL){
		sockService = new SocketService();
	}
	return sockService;
}
SocketService::~SocketService(){}

SocketService::attachHandle(int handle, Socket*sockInfo)
{
	char ch = "1";
	
	sockHandle.insert(make_pari(handle, sockInfo));

	FD_SET(handle, EVENT_READ);
	FD_SET(handle, EVENT_WRITE);
	FD_SET(handle, EVENT_EXCEPT);

	if (handle > maxFD){
			pritnf("handle is bigger than maxFD(%d)\n", maxFD);
			maxFD = handle;
	}
	write(mcntlPipe[1], &ch, 1);


}
	detachHandle(int)
	updateFD();
void SocketService::sendNotify()
{
}
void SocketService::threadImp()
{
	int nRes;
	int maxSignal;

	for(;;){
		memcopy(stReadFSTmp,stReadFS);
		memcopy(stwriteFSTmp,stReadFS);
		memcopy(stExceptFSTmp,stReadFS);

		nRes = select(maxFD+1,&stReadFSTmp, &stWriteFSTmp, &stExceptFSTmp);
		maxSignal = nRes;

		for(int i = 0,; i<sockHandle.size ; i++){
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
