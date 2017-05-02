#include "protocol_test.h"
#include "package.h"
#include "client_test.h"
#define CLIENT_CMD "client"
#define SERV_CMD   "server"

static bool callback_port(const char *p) {
    g_port = strdup(p);
    return true;
}

static bool callback_ip(const char *ip) {
    g_ip = strdup(ip);
    return true;
}

static bool callback_db(const char *db) {
    g_db_path = strdup(db);
    return true;
}

static bool callback_cert(const char *cert) {
    g_cert = strdup(cert);
    return true;
}

static const mopts_t    opts[] = {
    {
        .opt = 'p',
        .s_opt = "port",
        .desc = "Port",
        .take_arg = true,
        .callback = &callback_port,
        .usage = "PORT"
    },
    {
        .opt = 'a',
        .s_opt = "ip",
        .desc = "IP Address",
        .take_arg = true,
        .callback = &callback_ip,
        .usage = "IP"
    },
    {
        .opt = 'd',
        .s_opt = "database",
        .desc = "Database Path",
        .take_arg = true,
        .callback = &callback_db,
        .usage = "DB_FILE"
    },
    {
        .opt = 'c',
        .s_opt = "cert",
        .desc = "PEM Certificate path",
        .take_arg = true,
        .callback = &callback_cert,
        .usage = "PEM_FILE"
    },
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
    g_cert = NULL;
    set_program_name("protocol");
    read_opt(ac, av, opts, &list);

    if (g_port == 0 || g_ip == NULL || g_db_path == NULL || g_cert == NULL)
        opt_help(opts, 1);
    list_for_each(list, tmp, ptr) {
        if (strcmp(ptr, CLIENT_CMD) == 0)
            begin_client_test();
        else if (strcmp(ptr, SERV_CMD) == 0)
            begin_server_test();
    }
    return 0;
}
