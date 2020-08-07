#ifndef     _DATALINK_H
#define     _DATALINK_H

#define BUF_SIZE 1024

class DataLink
{
    char buffer[BUF_SIZE];
    unsigned int write;
    unsigned int read;

    int size;

    public:
    DataLink() : write(0), read(0), size(0) {};
    ~DataLink() {};
    int put(int size, char* buf);
    int get(int size, char*buf);
    void commit(int size);
    int getSize();
    
};
#endif
