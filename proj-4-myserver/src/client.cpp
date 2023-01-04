#include "client.h"

namespace netintergration
{

ChatClient::ChatClient(DataDecoderBase *decoder, const char *ip, int port)
{
    this->_client_name = readClientName();
    this->_ip = ip;
    this->_port = port;
    this->_buffer.len = 0;
}

ChatClient::~ChatClient()
{
    close(fds[0]);
    close(fds[1]);
}

const char *ChatClient::readClientName()
{
    std::string name;
    log_info("Please Enter your name: ", false);
    getline(std::cin, name);
    return name.c_str();
}

bool ChatClient::Init()
{
    if (!NetProxyBase::Init()) {
        return false;
    }
    return true;
}

void ChatClient::UnInit()
{
    event_base_dispatch(ev_base);
    NetProxyBase::UnInit();
}

void ChatClient::Close()
{
    ClientSocketBase::Close();
    if (_buff_ev != NULL) {
        bufferevent_setcb(_buff_ev, NULL, NULL, NULL, NULL);
        bufferevent_free(_buff_ev);
        _buff_ev = NULL;
    }
}

void ChatClient::read_cb(struct bufferevent *buff_ev, void *arg)
{
    // log_info("[CLIENT read_cb invoked]", true);
    ChatClient *client = (ChatClient *)arg;
    Message msg = client->_buffer;
    size_t size = bufferevent_read(buff_ev, (void *)msg.buf, BUFF_SIZE);
    std::cout << YEL "[SERVER]: " RESET << msg.buf << std::endl;
}

void ChatClient::write_cb(struct bufferevent *buff_ev, void *arg)
{
    // log_info("[CLIENT write_cb invoked]", true);
    ChatClient *client = (ChatClient *)arg;
    
}

void ChatClient::event_cb(struct bufferevent *buff_ev, short events, void *arg)
{
    ChatClient *client = (ChatClient *)arg;
    if(events & BEV_EVENT_CONNECTED) {
        log_info("[Connection successed]", true);
        return;
    } else if (events & BEV_EVENT_EOF) {
        log_info("[Connection closed]", true);
        client->Close();
    } else if (events & BEV_EVENT_ERROR) {
        log_error("[Connection error]", true);
        client->Close();
    }
    bufferevent_setcb(buff_ev, NULL, NULL, NULL, NULL);
    bufferevent_free(buff_ev);
}

bool ChatClient::Connect()
{
    struct sockaddr_in servaddr;
    int flag;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(_ip);
    servaddr.sin_port = htons(_port);

    _buff_ev = bufferevent_socket_new(ev_base, -1, BEV_OPT_CLOSE_ON_FREE);
    if (_buff_ev == NULL) {
        Close();
        return false;
    }
    flag = bufferevent_socket_connect(_buff_ev, (struct sockaddr *) &servaddr, 
                                      sizeof(servaddr));
    if (flag == -1) {
        Close();
        return false;
    }
    bufferevent_setcb(_buff_ev, read_cb, write_cb, event_cb, this);
    bufferevent_enable(_buff_ev, EV_READ | EV_WRITE);
    flag = event_base_dispatch(ev_base);
    if (flag == -1) {
        Close();
        return false;
    }
    return true;
}

void ChatClient::OnDecoder()
{
    _decoder->Process(this);
}

void ChatClient::Run()
{
    log_info("[CLIENT initializing...]", true);
    if (!Init()) {
		log_error("client->Init() error", true);
		return;
	}
    log_info("[CLIENT running...]", true);
    if (!Connect()) {
		log_error("client->Connect() error", true);
		return;
	}
    log_info("[CLIENT uninitializing...]", true);
	UnInit();
}

bool ChatClient::sendMessage(std::string buf)
{
    int flag = bufferevent_write(_buff_ev, buf.c_str(), buf.size());
    if (flag == -1) {
        log_error("CLIENT sending Message error", true);
        return false;
    }
    return true;
}

int run_client()
{
    DataDecoder_helloworld *decoder = new DataDecoder_helloworld();
    ChatClient *client = new ChatClient(decoder, IP, PORT);
    
    client->RunThread();
    while (1) {
        std::string buf;
        cin >> buf;
        int flag = client->sendMessage(buf);
    }
    client->Join();
    log_info("[CLIENT killed]", true);

    delete decoder;
    delete client;
    return 0;
}

}