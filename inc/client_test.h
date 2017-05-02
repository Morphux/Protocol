#ifndef CLIENT_TEST_H
# define CLIENT_TEST_H
# include <morphux.h>
# include <libmpm.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <strings.h>
# include <netdb.h>
# include "package.h"
# include <mbedtls/net.h>
# include <mbedtls/ssl.h>
# include <mbedtls/entropy.h>
# include <mbedtls/debug.h>
# include <mbedtls/ctr_drbg.h>
# include <mbedtls/certs.h>
# include <mbedtls/error.h>
# include "../lib/inc/package.h"

# define READ_TIMEOUT(sockfd, ssl, ret, size, sec, r_n) fd_set set; \
                        struct timeval timeout; \
                        int rv; \
                        FD_ZERO(&set); \
                        FD_SET((sockfd)->fd, &set); \
                        timeout.tv_sec = sec; \
                        timeout.tv_usec = 0; \
                        rv = select((sockfd)->fd + 1, &set, NULL, NULL, &timeout); \
                        TEST_ASSERT(rv > 0, "Timeout on read"); \
                        r_n = mbedtls_ssl_read(ssl, ret, size); \
                        if (r_n == 0) { (sockfd)->fd = 0; }


void    begin_client_test(void);

char    *g_port;
char    *g_ip;
char    *g_db_path;
char    *g_cert;

#endif /* CLIENT_TEST_H */
