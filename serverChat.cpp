#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "msgQueue.h"

#define PORT 9000

struct chatMsg
{
    enum cmdtype
    {
        NETWORK_EVENT,
        USER_EVENT,
    }
    cmdtype cmd;
    union cmd_msg{
        int netEvent;
        char* data;
    }
}

void sendEvent(socketEvent sockEvent){
    chatMsg msg;
    msg.cmd = NETWORK_EVENT;
    msg.cmd_msg.netEvent = sockEvent;
    msg.putQ();
}

void* userThread(void* data)
{
    chatMsg msg;
    char buf[128];
    
    while(true){
        scanf("%s", buf);

        char* data = malloc(sizeof(buf));
        memset(data, 0x00, szeof(data));
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
    
    msgQueue<chatMsg> msgQue;
    chatMsg cmd;

    SocketService::socketEvent sockEvent;
    Socket::socketState sockState;

    TCPServSocket* sock = new TCPServSocket();

    sock.open(&sin);

    while(true){
        msgQue.wait();
        while(!msgQue.empty()){
            cmd = msgQue.qetQ();
            sockState = sock->getState();
            switch(cmd.cmdtype){
                case USER_EVENT:
                    switch(sockState){
                        case ESTABLISHED:
                            sock.send(cmd->data, strlen(cmd->data));

                        case CLOSED:
                        case LISTEN:
                        case CONNECT:
                        case CLOSING:
                        case default:
                            pritnf("RX user event in Error state");
                            free(cmd->data);
                            break;

                    }
                case NETWORK_EVENT:
                    sockEvent = cmd->cmd_msg.netEvent;         
                    switch(sockState){
                        case CLOSED:
                            if (sockEvent & READ_EVNET){

                            }else if (sockEvent & WRITE_EVNET){


                            }else if (sockEvent & EXCEPT_EVNET){

                            } else {
                                printf("CLOSED state wrong event");
                            }
                            break;


                        case LISTEN:
                            if (sockEvent & READ_EVNET){
                                sock->accept();
                                sock->setCallback(sendEvent)

                            }else if (sockEvent & WRITE_EVNET){
                                printf("LISTEN state WRITE wrong event");


                            }else if (sockEvent & EXCEPT_EVNET){
                                sock->close();

                            } else {
                                printf("LISTEN state wrong event");
                            }
                            break;

                        case CONNECT:
                            if (sockEvent & READ_EVNET){

                            }else if (sockEvent & WRITE_EVNET){


                            }else if (sockEvent & EXCEPT_EVNET){

                            } else {
                                printf("CONNECT state wrong event");
                            }
                            break;
                        case ESTABLISHED:
                            if (sockEvent & READ_EVNET){

                            }else if (sockEvent & WRITE_EVNET){


                            }else if (sockEvent & EXCEPT_EVNET){

                            } else {
                                printf("ESTABLISHED state wrong event");
                            }
                            break;

                        case CLOSING:
                            if (sockEvent & READ_EVNET){

                            }else if (sockEvent & WRITE_EVNET){


                            }else if (sockEvent & EXCEPT_EVNET){

                            } else {
                                printf("CLOSING state wrong event");
                            }
                            break;
                        case default:
                            printf("RX user event in Error state");
                            break;

                    }
            }
        }
    }
}


