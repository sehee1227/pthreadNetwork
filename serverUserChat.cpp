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
    USER_END,

};

union cmd_msg{
    int netEvent;
    char* data;
};

msgQueue<chatMsg> servInputQue;
CondMgr servThrStart;

DataLink dLink;

void* servInputThread(void* data)
{
    chatMsg msg;
    char buf[128];

    printf("Start user Input thread\n");
    
    while(true){
        //scanf("%s", buf);

        fgets(buf, sizeof(buf), stdin);
        int len = strlen(buf);

        size_t EndLen = strlen(EXIT);
        char temp[6];
        if (EndLen == len){
            for(int i=0; i<(int)len; i++){
                temp[i] = toupper(*(buf+i));
            }

            if (strcmp(temp, EXIT) == 0){
                msg.cmd = USER_END;
                servInputQue.putQ(msg);
                continue;
            }
        }

        // printf("User Input len: %d, string: %s\n", len, buf);
        int res = dLink.put(len, buf);

        if (res != len){
            printf("Can't write to DataLink...User Input: %d, DataLink Write:%d\n", len, res);
        }

        msg.cmd = USER_EVENT;
        servInputQue.putQ(msg);

    }
}

void sendServChatEvent(int  sockEvent){
    chatMsg msg;
    msg.cmd = NETWORK_EVENT;
    msg.cmd_msg.netEvent = sockEvent;
    // printf("send NETWORK EVENT: %d\n", sockEvent);
    servInputQue.putQ(msg);
}

void* serverUserChat(void* data)
{
    pthread_t thr;
    int sockEvent;
    int sockState;
    int recvCnt;
    int sendByte;
    int datalen;

    int sockFd = *(int*) data;

    chatMsg msg;



    if(pthread_create(&thr, NULL, servInputThread, NULL) != 0){
		printf("Fail to pthread_create\n");
	}
    TCPSocket* sock = new TCPSocket(sockFd);

    sock->setCallback(sendServChatEvent);
    sock->setEvent(READ_EVENT);

    while(true){
        servInputQue.wait();
        while(!servInputQue.empty()){
            msg = servInputQue.qetQ();
            sockState = sock->getState();

            // printf("socket sate:%d\n", sockState);

            if (msg.cmd == USER_END){
                printf("serverChat socket Close() by User action\n");
                sock->Close();
                goto EXITLOOP;

            } 
            else if(msg.cmd == NETWORK_EVENT){
                // if (msg.cmd_msg.netEvent == READ_EVENT){
                //     printf("NETWORK_EVENT: READ\n");
                // }
                // if (msg.cmd_msg.netEvent == WRITE_EVENT){
                //     printf("NETWORK_EVENT: WRITE\n");
                // }
                // if (msg.cmd_msg.netEvent == EXCEPT_EVENT){
                //     printf("NETWORK_EVENT: EXCEPT\n");
                // }
            }

            if (sockState == ESTABLISHED){
                // printf("State : ESTABLISHED\n");  
                switch(msg.cmd){
                    case USER_EVENT:
                    {
                        char buf[128];
                        int size;
                        size = dLink.getSize();

                        if (sizeof(buf) < size){
                            size = sizeof(buf);
                        }
                        
                        dLink.get(size, buf);
                        sendByte = sock->Send(buf, size);
                        dLink.commit(sendByte-1);
                        buf[size] = '\0';
                        // printf("Network send len: %d, string: %s\n", sendByte-1, buf);
                        // printf("---------------------------------------------------\n");



                        // if(sendByte-1 < size){
                        //     printf("serverChat pendQ push_back %d bytes\n", (datalen-sendByte));
                        //     sock->setEvent(READ_EVENT | WRITE_EVENT );
                        // }

                        break;
                    }
                    case NETWORK_EVENT:
                        sockEvent = msg.cmd_msg.netEvent;
                        if (sockEvent & READ_EVENT){

                            recvCnt = sock->Recv(recBuf, RECBUF_SIZE);

                            if (recvCnt == 0){
                                printf("serverChat socket Close() by remote\n");
                                sock->Close();
                                // break;
                                goto  EXITLOOP;
                            }
                            if (recvCnt > 0){
                                recBuf[recvCnt-1] = '\0';
                                printf("serverChat recvCnt: %d\n", recvCnt);
                                printf("-->%s", recBuf);
                            }

                        }
                        if (sockEvent & WRITE_EVENT){
                            char buf[128];
                            int size;
                            size = dLink.getSize();
                            
                            if (sizeof(buf) < size){
                                size = sizeof(buf);
                            }   
                            
                            dLink.get(size, buf);
                            sendByte = sock->Send(buf, size);
                            dLink.commit(sendByte-1);


                            if(sendByte-1 < size){
                                printf("serverChat pendQ push_back %d bytes\n", (datalen-sendByte));
                                sock->setEvent(READ_EVENT | WRITE_EVENT );
                            }else{

                                printf("serverChat ESTABLISEHD WRITE EVENT \n");
                                sock->setEvent(READ_EVENT );

                            }

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

    delete sock;
    return NULL;
}


