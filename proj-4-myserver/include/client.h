#pragma once

#include "clientsocket.h"
#include "netproxy.h"
#include "thread.h"

#include "event2/event.h"
#include "event2/listener.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "until.h"
#include "log.h"

using namespace logger;

namespace netintergration
{

struct ClientPack {
    const char _name;
    Message message;
    // Time time_stamp;
};

class ChatClient : public NetProxyBase, public ClientSocketBase, public Thread
{
public:
    ChatClient(DataDecoderBase *decoder, const char *ip = IP, int port = PORT);
    virtual ~ChatClient() override;
    bool Init();
    void UnInit();
    bool Connect();
    virtual void OnDecoder() override;
    virtual void Run() override;
    inline int RunThread() { return Thread::Start(); }
    virtual void Close() override;
    // TESTING
    bool sendMessage(std::string buf);
    
    inline const char *getClientName() { return _client_name; }
public:
    Message _buffer;
private:
    const char *readClientName();
    static void read_cb(struct bufferevent *buff_ev, void *argu);
    static void write_cb(struct bufferevent *buff_ev, void *argu);
    static void event_cb(struct bufferevent *buff_ev, short events, void *argu);
private:
    const char *_client_name;
    const char *_ip;
    int _port;
    DataDecoderBase *_decoder;
    struct bufferevent *_buff_ev;
};

int run_client();

}
