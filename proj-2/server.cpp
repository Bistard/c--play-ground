#include <iostream>
#include <cstring>

#include <sys/socket.h>

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "epoll_tools.h"

using namespace std;

int main() {
    
    // set up socket for the server
    int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sock == -1)
        error("server_sock erorr");

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
    if (listen(server_sock, 20) == -1)
        error("listen to the client erorr");

    // create an epoll instance
    int epoll_fd;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
        error("epoll_fd created error");

    epoll_add(epoll_fd, server_sock, true);

    // main loop
    char reading_buffer[MAX_READING_BUFFER];

    while (true) {
        int success_event_count = epoll_wait(epoll_fd, events, 
                                             MAX_EPOLL_EVENTS, 
                                             INFINITE_WAIT_TIME);
        if (success_event_count < 0)
            error("epoll_wait error");
        
        // some sockets are ready, process each one of them
        for (int i = 0; i < success_event_count; i++) {
            
            int sock_fd = events[i].data.fd;
            int client_sock;
            
            if (sock_fd == server_sock) { // request for new connection
                
                // accept the client request
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_size = sizeof(clnt_addr);
                int client_sock = accept(server_sock, 
                                         (struct sockaddr *) &clnt_addr,
                                         &clnt_addr_size);
                if (client_sock == -1)
                    error("accept client request erorr");

                epoll_add(epoll_fd, client_sock, true);

                // output connection info
                cout << "welcome new guest!" << endl;
                cout << "client connection from: " << inet_ntoa(clnt_addr.sin_addr)
                     << " port: " << ntohs(clnt_addr.sin_port) 
                     << " clientfd = " << client_sock << endl;
                
            } else { // data from existing connection, process it
                
                int read_size = read(sock_fd, reading_buffer, MAX_READING_BUFFER);
                if (read_size == -1)
                    error("server reading error");
                
                cout << "server received message: " << reading_buffer;
                
                cout << "server echoing..." << endl;
                int written_size = write(sock_fd, reading_buffer, MAX_READING_BUFFER);
                if (written_size == -1)
                    error("server writing error");
            }
        }
    }

    // close the connection
    close(server_sock);
    close(epoll_fd);
    return 0;

}
