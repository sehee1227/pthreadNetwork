#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

extern void clientChat(char*);
extern void serverChat(const char*);

int main(int argc, char* argv[])
{
    const char* server = "SERVER";
    const char* client = "CLIENT";
    char buf[16] = {0,};
    int i = 0;
    
    if (argc > 1){
        while(argv[1][i]){
            buf[i] = toupper(argv[1][i]);
            i++;
        }
    }

    if (strcmp(server, buf) == 0){
        printf("Server\n");
        serverChat("SERVER");
    } else if (strcmp(client, buf) == 0){
        printf("Client\n");
        clientChat(argv[2]);
    } else {
        printf("Something wrong\n");
    }

    return 0;

}
