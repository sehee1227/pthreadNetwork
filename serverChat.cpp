#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "msgQueue.h"
#include "socketservice.h"
#include "TCPServSocket.h"
#include "datalink.h"

#define PORT 9000
#define RECBUF_SIZE 2048 

char recBuf[RECBUF_SIZE+1];
const char* EXIT = "EXIT\n";


struct chatMsg
{
    int cmd;
    union{
        int netEvent;
        char* data;
    }cmd_msg;
};

enum cmdtype
{
    USER_EVENT,
    NETWORK_EVENT,

};

union cmd_msg{
    int netEvent;
    char* data;
};

msgQueue<chatMsg> msgQue;


void sendEvent(int  sockEvent){
    chatMsg msg;
    msg.cmd = NETWORK_EVENT;
    msg.cmd_msg.netEvent = sockEvent;
    printf("sendEvent: %d\n", sockEvent);
    msgQue.putQ(msg);
}

void* userThread(void* data)
{
    chatMsg msg;
    char buf[128];

    printf("Start user thread\n");
    
    while(true){
        // scanf("%s", buf);

        fgets(buf, sizeof(buf), stdin);
        int len = strlen(buf);

        char* data = (char *)malloc(len);
        memset(data, 0x00, len);
        strncpy(data, buf, len);

        msg.cmd = USER_EVENT;
        msg.cmd_msg.data = data;
        msgQue.putQ(msg);
    }
}
void serverChat(const char *addr)
{
    pthread_t thr;
    int sockEvent;
    int sockState;
    int recvCnt;
    int sendByte;
    int datalen;

    chatMsg msg;
    DataLink sdlink;

    std::list<char*> pendServQ;
    
    if(pthread_create(&thr, NULL, userThread, NULL) != 0){
		printf("Fail to pthread_create\n");
	}
    
    TCPServSocket* sock = new TCPServSocket();

    if (sock->Open(addr, PORT) == false){
        printf("Fail to open\n");
        return;
    }
    sock->setCallback(sendEvent);

    while(true){
        msgQue.wait();
        while(!msgQue.empty()){
            msg = msgQue.qetQ();
            sockState = sock->getState();

            if (msg.cmd == USER_EVENT){
                char buf[5];
                size_t len = strlen(EXIT);
                if (strlen(msg.cmd_msg.data) == len){
                    for(int i=0; i<(int)len; i++){
                        buf[i] = toupper(*(msg.cmd_msg.data+i));
                    }
                    if (strcmp(buf, EXIT) == 0){
                        printf("serverChat socket Close() by User action\n");
                        sock->Close();
                    }
                }
                sdlink.put(strlen(msg.cmd_msg.data), msg.cmd_msg.data);
                printf("USER_EVENT length:%d, %s\n", (int)strlen(msg.cmd_msg.data), msg.cmd_msg.data);

                free(msg.cmd_msg.data);

            } else if(msg.cmd == NETWORK_EVENT){
                if (msg.cmd_msg.netEvent == READ_EVENT){
                    printf("NETWORK_EVENT: READ\n");
                }
                if (msg.cmd_msg.netEvent == WRITE_EVENT){
                    printf("NETWORK_EVENT: WRITE\n");
                }
                if (msg.cmd_msg.netEvent == EXCEPT_EVENT){
                    printf("NETWORK_EVENT: EXCEPT\n");
                }
            }

            if (sockState == CLOSED ){
                printf("State : CLOSED\n");
                switch(msg.cmd){
                    case USER_EVENT:
                            printf("serverChat user event in CLOSED, Error \n");
                            break;
                    case NETWORK_EVENT:
                            printf("serverChat Nework event in CLOSED, Error \n");    
                            break;  
                }            
            }else if (sockState == LISTEN){
                printf("State : LISTEN\n");
                switch(msg.cmd){
                    case USER_EVENT:
                            printf("serverChat user event in LISTEN, Error \n");
                            break;
                    case NETWORK_EVENT:
                        sockEvent = msg.cmd_msg.netEvent;
                        if (sockEvent & READ_EVENT){
                            printf("serverChat LISTEN Accept()\n");
                            sock->Accept();
                        }
                        if (sockEvent & WRITE_EVENT){
                            printf("LISTEN state WRITE wrong event\n");

                        }
                        if (sockEvent & EXCEPT_EVENT){
                            printf("serverChat LISTEN Close()\n");
                            sock->Close();
                        } 
                        break;
                }
            } else if (sockState == ESTABLISHED){
                printf("State : ESTABLISHED\n");  
                switch(msg.cmd){
                    case USER_EVENT:
                    {
                        datalen = sdlink.getSize();
                        char* buf = (char*)malloc(datalen);
                        sdlink.get(datalen, buf);
                        sendByte = sock->Send(buf, datalen);
                        printf("serverChat Send %d bytes\n", sendByte);

                        sdlink.commit(sendByte);

                        if(sendByte < datalen){
                            printf("serverChat pendQ push_back %d bytes\n", (datalen-sendByte));
                            sock->setEvent(READ_EVENT | WRITE_EVENT | EXCEPT_EVENT);
                        }

                        break;
                    }
                    case NETWORK_EVENT:
                        sockEvent = msg.cmd_msg.netEvent;
                        if (sockEvent & READ_EVENT){
                            recvCnt = sock->Recv(recBuf, RECBUF_SIZE);

                            if (recvCnt == 0){
                                printf("serverChat socket Close() by remote\n");
                                sock->Close();
                                break;
                            }
                            recBuf[recvCnt] = '\0';
                            printf("serverChat recvCnt: %d\n", recvCnt);
                            printf("-->%s\n", recBuf);

                        }
                        if (sockEvent & WRITE_EVENT){
                            datalen = sdlink.getSize();
                            char* buf = (char*)malloc(datalen);
                            sdlink.get(datalen, buf);
                            sendByte = sock->Send(buf, datalen);
                            printf("serverChat Send %d bytes\n", sendByte);

                            sdlink.commit(sendByte);

                            if(sendByte < datalen){
                                printf("serverChat pendQ push_back %d bytes\n", (datalen-sendByte));
                                sock->setEvent(READ_EVENT | WRITE_EVENT | EXCEPT_EVENT);
                            } else{
                                sock->setEvent(READ_EVENT | EXCEPT_EVENT);
                            }

                        }
                        if (sockEvent & EXCEPT_EVENT){
                            printf("serverChat ESTABLISHED Close()\n");
                            sock->Close();
                        } 
                        break;
                }
           } else if(sockState == CLOSING){
                printf("State : CLOSING\n");
                switch(msg.cmd){
                    case USER_EVENT:
                        printf("serverChat user event in CLOSING, Error \n");
                        break;
                    case NETWORK_EVENT:
                        printf("serverChat Nework event in CLOSING, Error \n");    
                        break;  
                }
            }
        }
    }

}


