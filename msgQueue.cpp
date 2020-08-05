#include "cond.h"


template<typename T>
class msgQueue
{
	list<T> msglist;
	CondMgr cond;
public:
	msgQueue(){}
	~msgQueue()
	{
		while(!msglist.empty()){
			msgist.pop_front();
		}
	}
	T qetQ()
	{	
		T data = msglist.front();
		msglist.pop_front();
		return data;
	}
	void putQ(T data)
	{
		msglist.push_back(data);
		cond.signal();
	}
	void wait()
	{
		cond.wait();
	}

	empty()
	{
		msglist.empty();
	}
	
}