
///////////////////////////////////////////////////////////////////////////////

// This file include serval helper functions that relevant to epoll.h

#ifndef EPOLL_TOOLS_
#define EPOLL_TOOLS_

#include <iostream>
#include <sys/epoll.h>
#include <assert.h>

#define SERVER_IP "127.0.0.1"
#define MAX_EPOLL_EVENTS 10
#define MAX_READING_BUFFER 100
#define INFINITE_WAIT_TIME -1

#define error(msg) {perror(msg); exit(EXIT_FAILURE);}

void epoll_add(int epfd, int fd, bool enable_et) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) 
        event.events |= EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1)
        error("epoll add error\n");
}

#endif // EPOLL_TOOLS_