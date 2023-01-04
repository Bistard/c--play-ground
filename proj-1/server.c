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
    
    // set up socket for the server
    int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sock == -1) {
        perror("server_sock erorr\n");
        exit(EXIT_FAILURE);
    }

    // bind the socket with the IP & port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(6666);

    bind(server_sock, 
         (const struct sockaddr *) &server_addr, 
         (socklen_t) sizeof(server_addr));

    // listen to the client
    if (listen(server_sock, 20) == -1) {
        perror("listen to the client erorr\n");
        exit(EXIT_FAILURE);
    }

    // accept the client request
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int client_sock = accept(server_sock, 
                             (struct sockaddr *) &clnt_addr,
                             &clnt_addr_size); // server will be blocked
    if (client_sock == -1) {
        perror("accept client request erorr\n");
        exit(EXIT_FAILURE);
    }
    // send a message
    char message[] = "Hello There!";
    if (write(client_sock, message, sizeof(message)) == -1) {
        perror("writting message erorr\n");
        exit(EXIT_FAILURE);
    }

    // close the connection
    close(client_sock);
    close(server_sock);

    return 0;
}
