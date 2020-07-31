#ifndef SOCKET_H
#define SOCKET_H
class Socket
{
	enum socketType{
		SOCKET_UDP,
		SOCKET_TCP,
	};
	enum IPVersion{
			IPv4,
			IPv6,
	};
	enum socketState{
			CLOSED,
			LISTENING,
			ESTABLIED,
			CLOSING,
	};

	int socketFD;
	sockState 	state;
	IPVersion 	IPVer;
	socketTyupe sockType;

	fd_set 	readFd;
	fd_set 	writeFd;
	fd_set	exceptFd;

	fd_set 	readFdTmp;
	fd_set 	writeFdTmp;
	fd_set	exceptFdTmp;
public:
	Socket();
	~Socket();
	open();
	close();
	notifyReadEv();
	notifyWriteEv();
	notifyExceptEv();
}

#endif SOCKT_H
