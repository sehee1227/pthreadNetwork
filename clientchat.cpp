#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "msgQueue.h"

void clientChat(const char *addr)
{
    int status ;
    struct sockaddr_in sin;
    struct addrinfo *result;

    printf("addr: %s\n", addr);

/*
    hints.ai_flags = AI_PASSIVE;
//  hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
*/
    //status = getaddrinfo(argv[1],"80", &hints, &result);
    status = getaddrinfo(addr, 0, 0, &result);
    if (status != 0){
        perror("gethostbyname falied\n");
        printf("Status : %d\n", status);
        exit(0);
    }
    
    // for (rp = result, i=0; rp != NULL; i++){
    //     sin = (sockaddr_in*)rp->ai_addr;
    //     printf("addr(%d):%s\n", i+1, inet_ntoa(sin->sin_addr));
    //     rp = rp->ai_next;
    // }


    // sin.sin_addr = (sockaddr_in*)result->ai_addr;
    sin = *(sockaddr_in*)result->ai_addr;
    printf("addr:%s\n", inet_ntoa(sin.sin_addr));
	freeaddrinfo(result);
    return;
}


