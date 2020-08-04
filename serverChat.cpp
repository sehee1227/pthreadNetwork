#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>


#include "msgQueue.h"
#include "socketservice.h"
#include "TCPServSocket.h"

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
    NETWORK_EVENT,
    USER_EVENT,
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
    msgQue.putQ(msg);
}

void* userThread(void* data)
{
    chatMsg msg;
    char buf[128];
    
    while(true){
        scanf("%s", buf);

        char* data = (char *)malloc(sizeof(buf));
        memset(data, 0x00, sizeof(data));
        strncpy(data, buf, sizeof(data));

        msg.cmd = USER_EVENT;
        msg.cmd_msg.data = data;
        msgQue.putQ(msg);
    }
}
void serverChat(const char *addr)
{
    struct sockaddr_in sin;

    printf("addr: %s\n", addr);

    memset(&sin, 0x00, sizeof(struct sockaddr_in));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = inet_addr(addr);
    
    int sockEvent;
    int sockState;

    chatMsg msg;
    
    TCPServSocket* sock = new TCPServSocket();

    sock->open(&sin);

    while(true){
        msgQue.wait();
        while(!msgQue.empty()){
            msg = msgQue.qetQ();
            sockState = sock->getState();
            switch(msg.cmd){
                case USER_EVENT:
                    switch(sockState){
                        case ESTABLISHED:
                            sock->Send(msg.cmd_msg.data, strlen(msg.cmd_msg.data));

                        case CLOSED:
                        case LISTEN:
                        case CONNECT:
                        case CLOSING:
                            printf("RX user event in Error state");
                            free(msg.cmd_msg.data);
                            break;

                    }
                case NETWORK_EVENT:
                    sockEvent = msg.cmd_msg.netEvent;         
                    switch(sockState){
                        case CLOSED:
                            if (sockEvent & READ_EVENT){

                            }else if (sockEvent & WRITE_EVENT){


                            }else if (sockEvent & EXCEPT_EVENT){

                            } else {
                                printf("CLOSED state wrong event");
                            }
                            break;


                        case LISTEN:
                            if (sockEvent & READ_EVENT){
                                sock->Accept();
                                sock->setCallback(sendEvent);

                            }else if (sockEvent & WRITE_EVENT){
                                printf("LISTEN state WRITE wrong event");


                            }else if (sockEvent & EXCEPT_EVENT){
                                sock->Close();

                            } else {
                                printf("LISTEN state wrong event");
                            }
                            break;

                        case CONNECT:
                            if (sockEvent & READ_EVENT){

                            }else if (sockEvent & WRITE_EVENT){


                            }else if (sockEvent & EXCEPT_EVENT){

                            } else {
                                printf("CONNECT state wrong event");
                            }
                            break;
                        case ESTABLISHED:
                            if (sockEvent & READ_EVENT){

                            }else if (sockEvent & WRITE_EVENT){


                            }else if (sockEvent & EXCEPT_EVENT){

                            } else {
                                printf("ESTABLISHED state wrong event");
                            }
                            break;

                        case CLOSING:
                            if (sockEvent & READ_EVENT){

                            }else if (sockEvent & WRITE_EVENT){


                            }else if (sockEvent & EXCEPT_EVENT){

                            } else {
                                printf("CLOSING state wrong event");
                            }
                            break;

                    }
            }
        }
    }
}


