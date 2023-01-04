#include <iostream>
#include <cstring>

#include <sys/socket.h>

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "epoll_tools.h"

using namespace std;

int main() {
    
    // set up socket for the client
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1)
        error("client_sock erorr");

    // connect the server (send request)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(6666);

    if (connect(client_sock, 
                (const struct sockaddr *) &server_addr, 
                (socklen_t) sizeof(server_addr)) == -1)
        error("server connection error");
    
    // main loop
    char reading_buffer[MAX_READING_BUFFER];

    while (true) {
        
        fgets(reading_buffer, MAX_READING_BUFFER, stdin);
        cout << "user input is: " << reading_buffer;
        int written_size = write(client_sock, reading_buffer, MAX_READING_BUFFER);
        if (written_size == -1)
            error("server writing error");
        
        int read_size = read(client_sock, reading_buffer, MAX_READING_BUFFER);
        if (read_size == -1)
            error("server reading error");

        cout << "echoing: " << reading_buffer << endl;
    }

    // close the connection
    close(client_sock);

    return 0;


    
}
