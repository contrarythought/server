#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "common.h"


#define MAXLINE 100

unsigned PORT;

void usage(char *program, char *instr) {
    fprintf(stderr, "%s %s\n", program, instr);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if(argc != 2) {
        usage(argv[0], "<greeting>");
    }

    char greeting[100];
    strcpy(greeting, argv[1]);

    unsigned listensock, clientsock;

    listensock = socket(AF_INET, SOCK_STREAM, 0);
    if(listensock == -1) {
        myerr("in socket creation");
    }

    int reuse = 1;
    if(setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
        myerr("In setting socket options");
    }

    printf("Port number:\n");
    scanf("%d", &PORT);

    struct sockaddr_in host_addr;
    memset((struct sockaddr *) &host_addr, 0, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listensock, (struct sockaddr *) &host_addr, sizeof(host_addr)) < 0) {
        myerr("In binding");
    }

    if(listen(listensock, 10) == -1) {
        myerr("In listening");
    }

    struct sockaddr_in client_addr;
    char buffer[MAXLINE];
    memset(buffer, 0, sizeof(buffer));
    
    while(1) {
        socklen_t len = sizeof(struct sockaddr);
        printf("Listening on port %d\n", PORT);

        clientsock = accept(listensock, (struct sockaddr *) &client_addr, &len);
        if(clientsock == -1) {
            myerr("In accepting client connection");
        }

        send(clientsock, greeting, strlen(greeting), 0);
        send(clientsock, "\n", 1, 0);
        
        int rec_len = recv(clientsock, buffer, sizeof(buffer), 0);
        int quit = 0;
        while(rec_len > 0) {
            if(strncasecmp(buffer, "quit", strlen("quit")) == 0) {
                quit = 1;
                break;
            }
            printf("Received %d bytes from client\n", rec_len);
            printf("%s", buffer);
            send(clientsock, "ECHO: ", strlen("ECHO "), 0);
            send(clientsock, buffer, sizeof(buffer),0);
            memset(buffer, 0, sizeof(buffer));
            rec_len = recv(clientsock, buffer, sizeof(buffer), 0);
        }
        if(quit)
            break;
    }
    close(clientsock);
    close(listensock);
    return 0;
}
