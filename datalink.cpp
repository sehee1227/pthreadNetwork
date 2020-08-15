#include <pthread.h>
#include <errno.h>
#include "datalink.h"

DataLink::DataLink(): write(0), read(0), size(0), readSize(0)
{
    m_pMutex = new pthread_mutex_t;
    if (m_pMutex){
        pthread_mutex_init((pthread_mutex_t*)m_pMutex, NULL);
    }
}

DataLink::~DataLink()
{
    if (m_pMutex){
        pthread_mutex_destroy((pthread_mutex_t*)m_pMutex);
    }
}

int DataLink::put(int length, char* buf)
{
    int len;
    if (length > (BUF_SIZE - this->size-1)){
        len = (BUF_SIZE - this->size-1);

    } else{
        len = length;
    }

    for (int i = 0; i<len; i++){
        buffer[(write+i)%BUF_SIZE] = buf[i];
    }

    while (pthread_mutex_lock((pthread_mutex_t*)m_pMutex) == EINTR);

    write = (write+len)%BUF_SIZE;
    size += len;

    pthread_mutex_unlock((pthread_mutex_t*)m_pMutex);

    return len;
}

int DataLink::get(int Size, char*buf)
{
    int len;

    while (pthread_mutex_lock((pthread_mutex_t*)m_pMutex) == EINTR);

    if (Size > (this->size)){
        len = (this->size);
    }else{
        len = Size;
    }

    pthread_mutex_unlock((pthread_mutex_t*)m_pMutex);

    for (int i = 0; i<len; i++){
        buf[i] = buffer[(read+i)%BUF_SIZE];
    }
    readSize += len;
    return len;
}


void DataLink::commit(int Size)
{
    read = (read+Size)%BUF_SIZE;

    pthread_mutex_unlock((pthread_mutex_t*)m_pMutex);
    
    if (Size > readSize){
        Size = readSize;
    }
    this->size -= Size;

    pthread_mutex_unlock((pthread_mutex_t*)m_pMutex);
}

int DataLink::getSize()
{
    return this->size;
}
