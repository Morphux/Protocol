#include "client_test.h"

static int      sockfd;

TEST(connect_1) {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT(sockfd > 0, "Cannot open socket");
    server = gethostbyname(g_ip);
    TEST_ASSERT(server != NULL, "Address doesn't exist");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
      (char *)&serv_addr.sin_addr.s_addr,
      server->h_length);
    serv_addr.sin_port = htons(g_port);
    if (connect(sockfd, &serv_addr, sizeof(serv_addr)) == -1)
        sockfd = 0;
    TEST_ASSERT(sockfd != 0, "Cannot connect to the server");
    return TEST_SUCCESS;
}

TEST(pkg_auth_1) {
    size_t          size;
    void            *ret;

    TEST_ASSERT(sockfd, "Server is not responding");
    ret = pkg_build_auth(&size, 0, 1);
    TEST_ASSERT(write(sockfd, ret, size) != 0,
        "Cannot send package to the server");
    return TEST_SUCCESS;
}

TEST(cleanup) {
    TEST_ASSERT(close(sockfd) != -1, "Cannot close socket");
    return TEST_SUCCESS;
}

void        begin_client_test(void) {
    reg_test("connect", connect_1);
    reg_test("auth", pkg_auth_1);
    reg_test("clean", cleanup);
    test_all();
    test_free();
}
