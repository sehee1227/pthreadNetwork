#include "datalink.h"

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
        write = write%BUF_SIZE;
    }
    size += len;
    return len;
}
int DataLink::get(int size, char*buf)
{
    int len;
    if (size > (this->size)){
        len = (this->size);
    }else{
        len = size;
    }

    for (int i = 0; i<len; i++){
        buf[i] = buffer[(read+i)%BUF_SIZE];
    }
    return len;
}


void DataLink::commit(int size)
{
    read = (read+size)%BUF_SIZE;
    this->size -= size;
}
int DataLink::getSize()
{
    return this->size;
}
