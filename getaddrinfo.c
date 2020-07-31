#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>

int main(int argc, char *argv[])
{
    int status;
    struct addrinfo *result, *rp;
    struct sockaddr_in *sin;
    struct addrinfo hints;

    printf("argv[1]: %s\n", argv[1]);

    memset(&hints, 0x00, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
//  hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(argv[1],"80", &hints, &result);
    status = getaddrinfo(argv[1],"80", 0, &result);
    if (status != 0){
        perror("gethostbyname falied\n");
        printf("Status : %d\n", status);
        exit(0);
    }

    sin = (void*)result->ai_addr;
    printf("addr:%s\n", inet_ntoa(sin->sin_addr));
	freeaddrinfo(result);
    return 0;
}


