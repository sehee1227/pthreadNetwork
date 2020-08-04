#ifndef _MSGQUEUE_H
#define _MSGQUEUE_H

#include <list>
#include "cond.h"

template<typename T>
class msgQueue
{
	std::list<T> msglist;
	CondMgr cond;

public:
//	msgQue(){}
	~msgQueue()
	{
	while(!msglist.empty()){
		msglist.pop_front();
	}
	};

T qetQ()
{	
	T data = msglist.front();
	msglist.pop_front();
	return data;
};
void putQ(T data)
{
	msglist.push_back(data);
	cond.signal();
};
void wait()
{
	cond.wait();
};

bool empty()
{
	return 	msglist.empty();
};
};

#endif
