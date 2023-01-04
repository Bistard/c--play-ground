#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/epoll.h>

#include <sys/socket.h>

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"

int main() {
    
    // set up socket for the client
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("client_sock erorr\n");
        exit(EXIT_FAILURE);
    }

    // connect the server (send request)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(6666);

    if (connect(client_sock, 
                (const struct sockaddr *) &server_addr, 
                (socklen_t) sizeof(server_addr)) == -1) {
        perror("server connection error\n");
        exit(EXIT_FAILURE);
    }
    
    // read the message
    char buf[100];
    read(client_sock, buf, sizeof(buf) - 1);

    printf("This is the message from the server: %s\n", buf);

    // close the connection
    close(client_sock);

    return 0;


    
}
