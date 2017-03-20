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

TEST(pkg_auth_1_write) {
    size_t          size;
    void            *ret;

    TEST_ASSERT(sockfd, "Server is not responding");
    ret = pkg_build_auth(&size, 1, 0);
    TEST_ASSERT(write(sockfd, ret, size) != 0,
        "Cannot send package to the server");
    return TEST_SUCCESS;
}

TEST(pkg_auth_1_read) {
    void        *ret, *expect;
    package_t   *pkg;
    auth_t      *auth;
    fd_set      set;
    size_t      r_n = 0, size, rv;
    struct timeval timeout;

    TEST_ASSERT(sockfd, "Server is not responding");
    expect = pkg_build_auth_ack(&size, 1, 0);
    ret = malloc(2048);

    FD_ZERO(&set);
    FD_SET(sockfd, &set);
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    rv = select(sockfd + 1, &set, NULL, NULL, &timeout);
    TEST_ASSERT(rv > 0, "Timeout on read");
    r_n = read(sockfd, ret, 2048);

    TEST_ASSERT(r_n == size, "Package returned size is wrong");
    TEST_ASSERT(memcmp(ret, expect, size) == 0, "Expected package is wrong");
    pkg = read_pkg(ret);
    TEST_ASSERT(pkg, "Can't read package");
    TEST_ASSERT(pkg->type == PKG_TYPE_AUTH_ACK, "Package type is wrong");

    auth = pkg->payload->member;
    TEST_ASSERT(auth->mpm_major_version == 1, "MPM major version is wrong");
    TEST_ASSERT(auth->mpm_minor_version == 0, "MPM minor version is wrong");
    free(ret);
    return TEST_SUCCESS;
}

TEST(pkg_req_get_pkg_1_write) {
    void        *ret;
    size_t      size;

    TEST_ASSERT(sockfd, "Server is not responding");
    ret = pkg_build_req_get_pkg(&size, 1, PKG_STABLE, "Test", "pkg", "v45.03");
    TEST_ASSERT(write(sockfd, ret, size), "Cannot send package to the server");
    return TEST_SUCCESS;
}

TEST(cleanup) {
    TEST_ASSERT(sockfd, "Server is not responding");
    TEST_ASSERT(close(sockfd) != -1, "Cannot close socket");
    return TEST_SUCCESS;
}

void        begin_client_test(void) {
    reg_test("connect", connect_1);
    reg_test("auth", pkg_auth_1_write);
    reg_test("auth", pkg_auth_1_read);
    reg_test("get_pkg", pkg_req_get_pkg_1_write);
    reg_test("clean", cleanup);
    test_all();
    test_free();
}
