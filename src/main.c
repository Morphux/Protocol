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

static void callback_db(const char *db) {
    g_db_path = strdup(db);
}

static const mopts_t    opts[] = {
    {'p', "port", "Port", true, &callback_port},
    {'a', "ip", "IP Address", true, &callback_ip},
    {'d', "database", "Database Path", true, &callback_db},
    ARGS_EOL
};

static void begin_server_test(void) {
    return ;
}

int     main(int ac, char **av) {
    mlist_t     *list = NULL, *tmp;
    char        *ptr;

    g_port = 0;
    g_ip = NULL;
    g_db_path = NULL;
    read_opt(ac, av, opts, &list);

    if (g_port == 0 || g_ip == NULL || g_db_path == NULL)
        m_panic("Missing arguments.");
    list_for_each(list, tmp, ptr) {
        if (strcmp(ptr, CLIENT_CMD) == 0)
            begin_client_test();
        else if (strcmp(ptr, SERV_CMD) == 0)
            begin_server_test();
    }
    return 0;
}
