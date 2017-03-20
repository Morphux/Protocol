#ifndef CLIENT_TEST_H
# define CLIENT_TEST_H
# include <morphux.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <strings.h>
# include <netdb.h>
# include "package.h"

# define READ_TIMEOUT(sockfd, ret, size, sec, r_n) fd_set set; \
                        struct timeval timeout; \
                        int rv; \
                        FD_ZERO(&set); \
                        FD_SET(sockfd, &set); \
                        timeout.tv_sec = sec; \
                        timeout.tv_usec = 0; \
                        rv = select(sockfd + 1, &set, NULL, NULL, &timeout); \
                        TEST_ASSERT(rv > 0, "Timeout on read"); \
                        r_n = read(sockfd, ret, size);


void    begin_client_test(void);

int     g_port;
char    *g_ip;

#endif /* CLIENT_TEST_H */
