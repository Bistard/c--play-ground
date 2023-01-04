#include "server.h"
#include "client.h"

#define SERVER "server"
#define CLIENT "client"

using namespace logger;
using namespace netintergration;

#include <vector>

int main(int argc, char *argv[]) 
{
    if (argc == 1) {
        log_info("[running as SERVER]", true);
        run_server();
    } else if (argc == 2 && !strcmp(argv[1], SERVER)) {
        log_info("[running as SERVER]", true);
        run_server();
    } else if (argc == 2 && !strcmp(argv[1], CLIENT)) {
        log_info("[running as CLIENT]", true);
        run_client();
    } else {
        log_error("wrong arguments", true);
    }
}

