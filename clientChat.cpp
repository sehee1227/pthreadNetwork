#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>


#include "msgQueue.h"
#include "socketservice.h"
#include "TCPCliSocket.h"

#define PORT 9000
#define RECBUF_SIZE 2048 

char recCliBuf[RECBUF_SIZE+1];

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

msgQueue<chatMsg> msgCliQue;


void sendCliEvent(int  sockEvent){
    chatMsg msg;
    msg.cmd = NETWORK_EVENT;
    msg.cmd_msg.netEvent = sockEvent;
    printf("sendEvent: %d\n", sockEvent);
    msgCliQue.putQ(msg);
}

void* userCliThread(void* data)
{
    chatMsg msg;
    char buf[128];

    printf("Start user thread\n");
    
    while(true){
        scanf("%s", buf);

        char* data = (char *)malloc(sizeof(buf));
        memset(data, 0x00, sizeof(data));
        strncpy(data, buf, sizeof(data));

        msg.cmd = USER_EVENT;
        msg.cmd_msg.data = data;
        msgCliQue.putQ(msg);
    }
}
void clientChat(const char *addr)
{
    pthread_t thr;
    int sockEvent;
    int sockState;

    chatMsg msg;

    int recvCnt;
    
    if(pthread_create(&thr, NULL, userCliThread, NULL) != 0){
		printf("Fail to pthread_create\n");
	}
    
    TCPCliSocket* sock = new TCPCliSocket();

    if (sock->Open(addr, PORT) == false){
        printf("Fail to open\n");
        return;
    }
    sock->setCallback(sendCliEvent);

    while(true){
        msgCliQue.wait();
        while(!msgCliQue.empty()){
            msg = msgCliQue.qetQ();
	    printf("get msg q, msg.cmd:%d, msg.cmd_msg.netEvent:%d\n", msg.cmd, msg.cmd_msg.netEvent);
            sockState = sock->getState();
	    printf("socket state: %d\n", sockState);
            switch(msg.cmd){
                case USER_EVENT:
                    switch(sockState){
                        case ESTABLISHED:
                            sock->Send(msg.cmd_msg.data, strlen(msg.cmd_msg.data));

                        case CLOSED:
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



                        case ESTABLISHED:
                            if (sockEvent & READ_EVENT){
                                recvCnt = sock->Recv(recCliBuf, RECBUF_SIZE);
                                recCliBuf[recvCnt] = '\0';
                                printf("serverChat recvCnt: %d\n", recvCnt);
                                printf("%s", recCliBuf);

                            }else if (sockEvent & WRITE_EVENT){


                            }else if (sockEvent & EXCEPT_EVENT){
                                sock->Close();

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


