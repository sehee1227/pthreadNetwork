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

#define PORT 9000
#define RECBUF_SIZE 1024 

char recCliBuf[RECBUF_SIZE+1];
const char* CEXIT = "EXIT\n";

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

msgQueue<chatMsg> msgCliQue;


void sendCliEvent(int  sockEvent){
    chatMsg msg;
    msg.cmd = NETWORK_EVENT;
    msg.cmd_msg.netEvent = sockEvent;
    // printf("sendEvent: %d\n", sockEvent);
    msgCliQue.putQ(msg);
}

void* userCliThread(void* data)
{
    chatMsg msg;
    char buf[128];

    printf("Start user thread\n");
    
    while(true){
        //scanf("%s", buf);

        fgets(buf, sizeof(buf), stdin);
        int len = strlen(buf);

        char* data = (char *)malloc(len);
        memset(data, 0x00, len);
        strncpy(data, buf, len);

        msg.cmd = USER_EVENT;
        msg.cmd_msg.data = data;
        msgCliQue.putQ(msg);
    }
}
void clientChat(char *addr)
{
    pthread_t thr;
    int sockEvent;
    int sockState;
    int recvCnt;
    int sendByte;
    int datalen;

    chatMsg msg;

    DataLink cdlink;

    int status ;
    struct sockaddr_in sin;
    struct addrinfo *result;

    printf("addr: %s\n", addr);

    status = getaddrinfo(addr, 0, 0, &result);
    if (status != 0){
        perror("gethostbyname falied\n");
        printf("Status : %d\n", status);
        exit(0);
    }

    sin = *(sockaddr_in*)result->ai_addr;
    strcpy(addr, inet_ntoa(sin.sin_addr));

	freeaddrinfo(result);
    
    if(pthread_create(&thr, NULL, userCliThread, NULL) != 0){
        printf("Fail to pthread_create\n");
    }
    
    TCPSocket* sock = new TCPSocket();

    if (sock->Open(addr, PORT, CLIENT) == false){
    // if (sock->Open(addr, PORT, CLIENT) == false){
        printf("Fail to open\n");
        return;
    }
    sock->setCallback(sendCliEvent);

    while(true){
        msgCliQue.wait();
        while(!msgCliQue.empty()){
            msg = msgCliQue.qetQ();
            sockState = sock->getState();

            if (msg.cmd == USER_EVENT){
                char buf[5];
                size_t len = strlen(CEXIT);
                if (strlen(msg.cmd_msg.data) == len){
                    for(int i=0; i<(int)len; i++){
                        buf[i] = toupper(*(msg.cmd_msg.data+i));
                    }
                    if (strcmp(buf, CEXIT) == 0){
                        printf("ClientChat socket Close() by User action\n");
                        sock->Close();
                        goto EXITLOOP;

                        return;
                    }
                }
                // cdlink.put(strlen(msg.cmd_msg.data), msg.cmd_msg.data);
                // printf("USER_EVENT length:%d, %s\n", (int)strlen(msg.cmd_msg.data), msg.cmd_msg.data);

                // free(msg.cmd_msg.data);

            } else if(msg.cmd == NETWORK_EVENT){
                // if (msg.cmd_msg.netEvent == READ_EVENT){
                //     printf("NETWORK_EVENT: READ\n");
                // } else if (msg.cmd_msg.netEvent == WRITE_EVENT){
                //     printf("NETWORK_EVENT: WRITE\n");
                // } else if (msg.cmd_msg.netEvent == EXCEPT_EVENT){
                //     printf("NETWORK_EVENT: EXCEPT\n");
                // }
            }

            if (sockState == CLOSED ){
                printf("State : CLOSED\n");
                switch(msg.cmd){
                    case USER_EVENT:
                        printf("ClientChat user event in CLOSED, Error \n");
                        break;
                    case NETWORK_EVENT:
                        printf("ClientChat Nework event in CLOSED, Error \n");    
                        break;  
                }            
            } else if (sockState == ESTABLISHED){
                // printf("State : ESTABLISHED\n");  
                switch(msg.cmd){
                    case USER_EVENT:
                    {
                        // datalen = cdlink.getSize();
                        // char* buf = (char*)malloc(datalen);
                        // printf("ClientChat before Send %d bytes: %s\n", sendByte, buf);
                        // cdlink.get(datalen, buf);
                        // sendByte = sock->Send(buf, datalen);
                        // printf("ClientChat Send %d bytes\n", sendByte);

                        // cdlink.commit(sendByte);

                        datalen = strlen(msg.cmd_msg.data);

                        // printf("ClientChat before Send %d bytes: %s\n", datalen, msg.cmd_msg.data);
                        sendByte = sock->Send(msg.cmd_msg.data, datalen);
                        // printf("ClientChat Send %d bytes\n", sendByte);
                          free(msg.cmd_msg.data);

                        // if(sendByte < datalen){
                        //     printf("ClientChat pendQ push_back %d bytes\n", (datalen-sendByte));
                        //     sock->setEvent(READ_EVENT | WRITE_EVENT );
                        // }

                        break;
                    }
                    case NETWORK_EVENT:
                        sockEvent = msg.cmd_msg.netEvent;
                        if (sockEvent & READ_EVENT){
                            recvCnt = sock->Recv(recCliBuf, RECBUF_SIZE);

                            if (recvCnt == 0){
                                printf("ClientChat socket Close() by remote\n");
                                sock->Close();
                                goto EXITLOOP;
                            }
                            else if(recvCnt >0){

                                recCliBuf[recvCnt] = '\0';
                                // printf("ClientChat recvCnt: %d\n", recvCnt);
                                printf("-->%s", recCliBuf);
                            }

                        } 
                        if (sockEvent & WRITE_EVENT){
                            // datalen = cdlink.getSize();
                            // char* buf = (char*)malloc(datalen);
                            // cdlink.get(datalen, buf);
                            // sendByte = sock->Send(buf, datalen);
                            // printf("ClientChat Send %d bytes\n", sendByte);

                            // cdlink.commit(sendByte);

                            // if(sendByte < datalen){
                            //     printf("ClientChat pendQ push_back %d bytes\n", (datalen-sendByte));
                            //     sock->setEvent(READ_EVENT | WRITE_EVENT | EXCEPT_EVENT);
                            // } else{
                            //     sock->setEvent(READ_EVENT | EXCEPT_EVENT);
                            // }
                            printf("serverChat ESTABLISEHD WRITE EVENT \n");

                            // sock->setEvent(READ_EVENT | EXCEPT_EVENT);
                        }
                        break;
                }
            } else if(sockState == CLOSING){
                printf("State : CLOSING\n");  
            }else if(sockState == CONNECT ){
                printf("State : CONNECT\n");
                switch(msg.cmd){
                    case USER_EVENT:
                        printf("ClientChat user event in CONNECT, Error \n");
                        break;
                    case NETWORK_EVENT:
                        sockEvent = msg.cmd_msg.netEvent;
                        if (sockEvent & READ_EVENT){
                            printf("ClientChat socket Connect\n");
                        }
                        if (sockEvent & WRITE_EVENT){
                            printf("ClientChat socket Connect()\n");
                            sock->Connect();
                        }
                        break;
                }
            }
        }
    }

    EXITLOOP:
    delete sock;
    return;
}

