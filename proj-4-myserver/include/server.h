#pragma once

#include <string>
#include <deque>

#include "client.h"
#include "netproxy.h"
#include "thread.h"
#include "datadecoder.h"

#include "event2/event.h"
#include "event2/listener.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"
#include "until.h"

#define PACKTIMEOUT 10
#define CONNTIMEOUT 10

using namespace logger;

namespace netintergration
{

class Server : public NetProxy<ClientSocket>, public Thread
{
public:
    Server(DataDecoderBase *decoder, const char *ip = IP, int port = PORT);
    virtual ~Server() override;
    bool Init();
    void UnInit();
    bool Start();
    virtual void Run() override;
    inline int RunThread() { return Thread::Start(); }
    
    int getClientCount();
    bool sendMessage(std::string buf);
    // UNUSED FOR NOW
    void PushMessage(Message message);
    Message PopMessage();
    void Notify();
private:
    static void accept_cb(struct evconnlistener *listener, int sock, 
                                 struct sockaddr *address, int socklen, 
                                 void *ctx);
    static void read_cb(struct bufferevent *bev, void *argu);
    static void write_cb(struct bufferevent *bev, void *argu);
    static void event_cb(struct bufferevent *bev, short events, void *argu);
public:
    Message _buffer;
private:
    const char *_ip;
    int _port;
    DataDecoderBase *_decoder;
    std::deque<Message> *_queue;
    struct evconnlistener *_listener;
    struct bufferevent *_buff_ev;
};

int run_server();

}
