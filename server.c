#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("%s <port>\n", argv[0]);
        return 1;
    }

    char *port = strdup(argv[1]);
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // this flag is used for addresses that servers can bind to

    struct addrinfo *bind_address;
    getaddrinfo(NULL, port, &hints, &bind_address);

    int listen_sock = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if(listen_sock == -1) {
        printf("Failed to create socket: %d\n", errno);
        return 1;
    }

    if(bind(listen_sock, bind_address->ai_addr, bind_address->ai_addrlen) == -1) {
        printf("Failed to bind: %d\n", errno);
        return 1;
    }
    
    if(listen(listen_sock, 10) == -1) {
        printf("Failed to to listen: %d\n", errno);
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_size = sizeof(client_address);
    int client_sock = accept(listen_sock, (struct sockaddr *) &client_address, &client_size);
    if(client_sock == -1) {
        printf("Failed to accept client connection: %d\n", errno);
        return 1;
    }

    printf("Client is connected...");
    char address_buffer[33];
    getnameinfo((struct sockaddr *) &client_address, client_size, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    char recv_buf[1024];
    int bytes_rec = recv(client_sock, recv_buf, sizeof(recv_buf), 0);
    if(bytes_rec == -1) {
        printf("Failed to receive request: %d\n", errno);
        return 1;
    }

    printf("%.*s", bytes_rec, recv_buf);

    printf("Sending response...\n");
    char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Connection: close\r\n"
    "Content-Type: text/plain\r\n\r\n"
    "Local time: ";
    int bytes_send = send(client_sock, response, strlen(response), 0);
    if(bytes_send == -1) {
        printf("Failed to send: %d\n", errno);
        return 1;
    }

    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_send = send(client_sock, time_msg, strlen(time_msg), 0);
    if(bytes_send == -1) {
        printf("Failed to send: %d\n", errno);
        return -1;
    }
    close(listen_sock);
    free(port);
    return 0;
}
