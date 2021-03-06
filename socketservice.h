#ifndef _SOCKETSERVICE_H
#define _SOCKETSERVICE_H
#include <list>
#include <pthread.h>
#include <sys/select.h>

#include "Socket.h"
#include "cond.h"

class Socket;

enum socketEvent {
	READ_EVENT = 0x01,
	WRITE_EVENT = 0x02,
	EXCEPT_EVENT = 0x04,
};

class SocketService 
{
	struct SockInfo{
		int sockFd; 	
		Socket* socket;
	};
	std::list<SockInfo> sockList;

	int mctrlPipe[2];
	int	mcntlEvent;
	int maxFd;
	int threadRef;
	bool stopThread;

	CondMgr sockCond;

	pthread_t thr;

	fd_set	stReadFS;
	fd_set	stWriteFS;
	fd_set	stExceptFS;

	fd_set	stReadFSTmp;
	fd_set	stWriteFSTmp;
	fd_set	stExceptFSTmp;
public:
	SocketService();
	static SocketService* getInstance();
	~SocketService();
	bool attachHandle(int, Socket*);
	void detachHandle(int);
	void updateEvent(int, int);
	void sendNotify(Socket*, int);
	static void* threadImp(void*);
	void terminateThread(void);
	void* run(void);
	bool isRunnung();
};
#endif
