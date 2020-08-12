#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "msgQueue.h"
#include "socketservice.h"
#include "TCPSocket.h"
#include "datalink.h"
#include "serverUserChat.h"

#define PORT 9000

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

msgQueue<chatMsg> msgServQue;

void sendEvent(int  sockEvent){
    chatMsg msg;
    msg.cmd = NETWORK_EVENT;
    msg.cmd_msg.netEvent = sockEvent;
    printf("send NETWORK EVENT: %d\n", sockEvent);
    msgServQue.putQ(msg);
}

void* userThread(void* data)
{

    printf("Start user thread\n");

    return serverUserChat(data);

}
void serverChat(const char *addr)
{
    pthread_t thr;
    int cliSock;
    int sockEvent;
    int sockState;
    int recvCnt;
    int sendByte;
    int datalen;

    chatMsg msg;
    DataLink sdlink;

    
    TCPSocket* sock = new TCPSocket();

    if (sock->Open(addr, PORT, SERVER) == false){
        printf("Fail to open\n");
        return;
    }
    sock->setCallback(sendEvent);

    while(true){
        msgServQue.wait();
        while(!msgServQue.empty()){
            msg = msgServQue.qetQ();
            sockState = sock->getState();

            if (msg.cmd == USER_EVENT){
                // sdlink.put(strlen(msg.cmd_msg.data), msg.cmd_msg.data);
                // printf("USER_EVENT length:%d, %s\n", (int)strlen(msg.cmd_msg.data), msg.cmd_msg.data);

                // free(msg.cmd_msg.data);

            } 
            else if(msg.cmd == NETWORK_EVENT){
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
                            cliSock = sock->Accept();

                            if(pthread_create(&thr, NULL, userThread, (void*) &cliSock) != 0){
                                printf("Fail to pthread_create\n");
                            }

                            goto EXITLOOP;

                        }
                        if (sockEvent & WRITE_EVENT){
                            printf("LISTEN state WRITE wrong event\n");

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
    EXITLOOP:
    pthread_join(thr, NULL);
    printf("end sever chat main sock->Close()\n");

    sock->Close();
    delete sock;

}


