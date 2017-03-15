#include "protocol_test.h"
#include "package.h"
#include "client_test.h"
#define CLIENT_CMD "client"
#define SERV_CMD   "server"

static void callback_port(const char *p) {
    g_port = atoi(p);
}

static void callback_ip(const char *ip) {
    g_ip = strdup(ip);
}

static const mopts_t    opts[] = {
    {'p', "port", "Port", true, &callback_port},
    {'a', "ip", "IP Address", true, &callback_ip},
    ARGS_EOL
};

static void begin_server_test(void) {
    return ;
}

int     main(int ac, char **av) {
    mlist_t     *list = NULL, *tmp;
    char        *ptr;

    read_opt(ac, av, opts, &list);
    list_for_each(list, tmp, ptr) {
        if (strcmp(ptr, CLIENT_CMD) == 0)
            begin_client_test();
        else if (strcmp(ptr, SERV_CMD) == 0)
            begin_server_test();
    }
    return 0;
}
