#include <vector>

#include "client.h"
#include "server.h"
#include "netproxy.h"
#include "event2/event.h"
#include "log.h"
#include "thread.h"

using namespace logger;
using namespace netintergration;

namespace netintergration
{

Server::Server(DataDecoderBase *decoder, const char *ip, int port) 
    : NetProxy<ClientSocket>(decoder)
{
    this->_decoder = decoder;
    this->_ip = ip;
    this->_port = port;
    this->_queue = new std::deque<Message>();
    this->_buffer.len = 0;
    this->_listener = NULL;
    this->_buff_ev = NULL;
}

Server::~Server()
{
    close(fds[0]);
    close(fds[1]);
}

bool Server::Init()
{
    if (!NetProxy<ClientSocket>::Init()) {
        return false;
    }
    return true;
}

void Server::UnInit()
{
    evconnlistener_free(_listener);
    NetProxy<ClientSocket>::UnInit();
} 

void Server::accept_cb(struct evconnlistener *listener, int fd, 
                       struct sockaddr *address, int socklen, void *arg)
{
    Server *server = (Server *)arg;
    // bufferevent *buff_ev = server->_buff_ev;
    // struct event_base *ev_base = server->ev_base;
    server->_buff_ev = bufferevent_socket_new(server->ev_base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!server->_buff_ev) {
        log_error("[construct bufferevent error]", true);
        event_base_loopbreak(server->ev_base);
        return;
    }
    bufferevent_setcb(server->_buff_ev, read_cb, write_cb, event_cb, server);
    bufferevent_enable(server->_buff_ev, EV_WRITE | EV_READ);
    log_info("[Connection successed]", true);
}

void Server::read_cb(struct bufferevent *buff_ev, void *arg)
{
    // log_info("SERVER read_cb invoked", true);
    char msg[BUFF_SIZE] = {0};
	bufferevent_read(buff_ev, msg, sizeof(msg));
	std::cout << YEL "[CLIENT]: " RESET << msg << std::endl;
}

void Server::write_cb(struct bufferevent *buff_ev, void *arg)
{
    // log_info("[SERVER write_cb is invoked]", true);
}

void Server::event_cb(struct bufferevent *buff_ev, short events, void *arg)
{
    if (events & BEV_EVENT_EOF) {
        log_info("[Connection closed]", true);
    } else if (events & BEV_EVENT_ERROR) {
        log_error("[Connection error]", true);
    }
    bufferevent_free(buff_ev);
}

bool Server::Start()
{
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(_ip);
    servaddr.sin_port = htons(_port);
    
    _listener = evconnlistener_new_bind(ev_base, accept_cb, this, 
                                        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                        -1, (struct sockaddr *) &servaddr, 
                                        sizeof(servaddr));
    if (!_listener) {
        return false;
    }

    if (!NetProxy<ClientSocket>::Start()) {
        return false;
    }
    return true;
}

void Server::PushMessage(Message message)
{
    _queue->push_back(message);
}

Message Server::PopMessage()
{
    if (_queue->empty()) {
        log_error("[No Message to pop]", true);
        exit(EXIT_CODE);
    }
    Message message = _queue->front();
    _queue->pop_front();
    return message;
}

void Server::Notify()
{
    Message message = PopMessage();
    write(NetProxyBase::fds[0], message.buf, message.len);
}

void Server::Run()
{
    log_info("[SERVER initializing...]", true);
    if (!Init()) {
		log_error("Server Init error", true);
		return;
	}
    log_info("[SERVER running...]", true);
    if (!Start()) {
		log_error("Server Start error", true);
		return;
	}
    log_info("[SERVER uninitializing...]", true);
	UnInit();
}

bool Server::sendMessage(std::string buf)
{
    // ClientMap *clients = GetClientMap();
    // clients->SendAll(buf.c_str(), buf.size());
    int flag = bufferevent_write(_buff_ev, buf.c_str(), buf.size());
    return flag;
}

int Server::getClientCount()
{
    return GetClientMap()->Size();
}

int run_server()
{
    DataDecoder_helloworld *decoder = new DataDecoder_helloworld();
    Server *server = new Server(decoder, IP, PORT);
    server->RunThread();
    
    while (1) {
        std::string buf;
        cin >> buf;
        server->sendMessage(buf);
        // std::cout << "[Total Clients: " << server->getClientCount() << std::endl;
    }
    
    server->Join();
    log_info("[SERVER killed]", true);

    delete decoder;
    delete server;
    return 0;
}

}