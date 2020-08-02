#ifndef _SOCKETSERVICE_H
#define _SOCKETSERVICE_H
#include <list>
#include <pthread.h>

class SocketService 
{
	struct SockInfo{
		int sockFd; 	
    	void* pInstance;
	}
	list<SockInfo> sockInfo;
	pthread_t sockServThread_t;

	int mctrlPipe[2];
	int	mcntlEvent;
	int maxFd;

	fd_set	stReadFS;
	fd_set	stWriteFS;
	fd_set	stEceptFS;

	fd_set	stReadFSTmp;
	fd_set	stWriteFSTmp;
	fd_set	stEceptFSTmp;
public:
	SocketService();
	static SocketService* getInstance();
	~SocketService();
	bool attachHandle(int, Socket*);
	void detachHandle(int);
	void updateFD();
	void sendNotify()
	void* threadImp(void*);

}
#endif
