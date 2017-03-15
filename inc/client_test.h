#ifndef CLIENT_TEST_H
# define CLIENT_TEST_H
# include <morphux.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include "package.h"

void    begin_client_test(void);

int     g_port;
char    *g_ip;

#endif /* CLIENT_TEST_H */
