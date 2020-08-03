#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include "cond.h"

CondMgr::CondMgr()
{

	m_pMutex = new pthread_mutex_t;
	m_pCond = new pthread_cond_t;

	if (m_pMutex){
		pthread_mutex_init((pthread_mutex_t*)m_pMutex, NULL);
	}
	if (m_pCond){
		pthread_cond_init((pthread_cond_t*)m_pCond, NULL);
	}
}

CondMgr::~CondMgr()
{
	if (m_pMutex){
		pthread_mutex_destroy((pthread_mutex_t*)m_pMutex);
	}
	if (m_pCond){
		pthread_cond_destroy((pthread_cond_t*)m_pCond);
	}
}

void CondMgr::wait()
{
	while (pthread_mutex_lock((pthread_mutex_t*)m_pMutex) == EINTR);
	while (pthread_cond_wait((pthread_cond_t*)m_pCond, (pthread_mutex_t*)m_pMutex) == EINTR);
	pthread_mutex_unlock((pthread_mutex_t*)m_pMutex);
}

bool CondMgr::wait_timeout(long waitTime)
{
	struct timespec ts;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	ts.tv_sec = tv.tv_sec + (waitTime / 1000);
	
	long addUSec = tv.tv_usec + (waitTime % 1000) * 1000;
    if ( addUSec >= 1000000){
        ts.tv_sec++;
        addUSec -= 1000000;
    }
    ts.tv_nsec = addUSec * 1000L;
        
    pthread_mutex_lock((pthread_mutex_t*)m_pMutex);
		
	while (pthread_cond_timedwait((pthread_cond_t*)m_pCond, (pthread_mutex_t*)m_pMutex, &ts) == EINTR);
    pthread_mutex_unlock((pthread_mutex_t*)m_pMutex);

}
void CondMgr::signal()
{
	if (m_pCond == NULL || m_pMutex == NULL){
        return;
    }

    while (pthread_mutex_lock((pthread_mutex_t*)m_pMutex) == EINTR);
    pthread_cond_signal((pthread_cond_t*)m_pCond);
    pthread_mutex_unlock((pthread_mutex_t*)m_pMutex);

}

