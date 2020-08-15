#ifndef     _DATALINK_H
#define     _DATALINK_H

#define BUF_SIZE 1024

class DataLink
{
    char buffer[BUF_SIZE];
    unsigned int write;
    unsigned int read;

    int size;
    int readSize;
    void *m_pMutex;

    public:
    DataLink();
    ~DataLink();
    int put(int size, char* buf);
    int get(int size, char*buf);
    void commit(int size);
    int getSize();
    int getWrite();
    int getRead();
    
};
#endif
