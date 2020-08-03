#ifndef _MSGQURUR_H
#define _MSGQURUR_H

template<typename T>
class msgQueue
{
	list<T> msglist;
	CondMgr cond;
public:
	msgQueue();
	~msgQueue();
	T qetQ();
	void putQ(T data);
	void wait();
	empty();
}

#endif