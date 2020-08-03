
msgQueue::~msgQueue()
{
	while(!msglist.empty()){
		msgist.pop_front();
	}
}
T msgQueue::qetQ()
{	
	T data = msglist.front();
	msglist.pop_front();
	return data;
}
void msgQueue::putQ(T data)
{
	msglist.push_back(data);
	cond.signal();
}
void msgQueue::wait()
{
	cond.wait();
}

msgQueue::empty()
{
	msglist.empty();
}
	