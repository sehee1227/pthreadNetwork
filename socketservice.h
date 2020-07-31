#ifndef _SOCKETSERVICE
#DEFINE _SOCKETSERVICE

class SocketService
{
	struct sockInfo{
		Socket* plistener; 	
	}
	map<int, sockInfo*> sockHandle;
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
	attachHandle(int, Socket*);
	detachHandle(int);
	updateFD();
	threadImp();

}
#endif
