#ifndef SOCKET_H
#define SOCKET_H
	enum socketState : int{
			CLOSED,
			LISTEN,
			CONNECT,
			ESTABLISHED,
			CLOSING,
	};
class Socket
{

	int socketFD;
	int 	state;

	fd_set 	readFd;
	fd_set 	writeFd;
	fd_set	exceptFd;

	fd_set 	readFdTmp;
	fd_set 	writeFdTmp;
	fd_set	exceptFdTmp;
public:
	void setState(int state)
	{
		this->state = state;
	}
	int getState()
	{
		return this->state;
	}
	virtual void notify(int);

};

#endif 
