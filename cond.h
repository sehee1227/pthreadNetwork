#ifndef COND_H
#define COND_H

class CondMgr{
    void *m_pCond;
    void *m_pMutex;

public:
    CondMgr();
    ~CondMgr();
    void wait();
    bool wait_timeout(long waitTime);
    void signal();
};

#endif
