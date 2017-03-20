#include "client_test.h"

static int              sockfd;
static database_t       *db;

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

TEST(connect_db) {
    u8_t        ret;

    db = mpm_database_open(&ret, g_db_path);
    TEST_ASSERT(ret != 0, "Cannot open the database");
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
    size_t      r_n = 0, size;

    TEST_ASSERT(sockfd, "Server is not responding");
    expect = pkg_build_auth_ack(&size, 1, 0);
    ret = malloc(2048);

    READ_TIMEOUT(sockfd, ret, 2048, 1, r_n);
    TEST_ASSERT_FMT(memcmp(ret, expect, size) == 0,
        "Expected package is wrong %s", print_package(expect, ret, size, r_n));
    pkg = read_pkg(ret);
    TEST_ASSERT(pkg, "Can't read package");
    TEST_ASSERT(pkg->type == PKG_TYPE_AUTH_ACK, "Package type is wrong");

    auth = pkg->payload->member;
    TEST_ASSERT(auth->mpm_major_version == 1, "MPM major version is wrong");
    TEST_ASSERT(auth->mpm_minor_version == 0, "MPM minor version is wrong");
    free(ret);
    return TEST_SUCCESS;
}

TEST(pkg_auth_2_write) {
    size_t          size;
    void            *ret;

    TEST_ASSERT(sockfd, "Server is not responding");
    ret = pkg_build_auth(&size, 1, 1);
    TEST_ASSERT(write(sockfd, ret, size) != 0,
        "Cannot send package to the server");
    return TEST_SUCCESS;
}

TEST(pkg_auth_3_write) {
    size_t          size;
    void            *ret;

    TEST_ASSERT(sockfd, "Server is not responding");
    ret = pkg_build_auth(&size, -1, -1);
    TEST_ASSERT(write(sockfd, ret, size) != 0,
        "Cannot send package to the server");
    return TEST_SUCCESS;
}

TEST(pkg_req_get_pkg_1_write) {
    void        *ret;
    size_t      size;

    TEST_ASSERT(sockfd, "Server is not responding");
    ret = pkg_build_req_get_pkg(&size, 0, PKG_STABLE, "", "", "");
    TEST_ASSERT(write(sockfd, ret, size), "Cannot send package to the server");
    return TEST_SUCCESS;
}

TEST(pkg_req_get_pkg_1_read) {
    void        *ret, *expect;
    package_t   *pkg;
    error_pkg_t *err;
    size_t      r_n = 0, size;

    TEST_ASSERT(sockfd, "Server is not responding");
    expect = pkg_build_error(&size, ERR_RES_NOT_FOUND, "Package not found");
    ret = malloc(2048);

    READ_TIMEOUT(sockfd, ret, 2048, 1, r_n);
    TEST_ASSERT_FMT(memcmp(ret, expect, size) == 0,
        "Expected package is wrong %s", print_package(expect, ret, size, r_n));
    pkg = read_pkg(ret);
    TEST_ASSERT(pkg, "Can't read package");
    TEST_ASSERT(pkg->type == PKG_TYPE_ERROR, "Package type is wrong");

    err = pkg->payload->member;
    TEST_ASSERT(err->error_type == ERR_RES_NOT_FOUND, "Error type is wrong");
    TEST_ASSERT(strcmp(err->err, "Package not found") == 0, "Error string is wrong");
    free(ret);
    return TEST_SUCCESS;
}

TEST(cleanup_co) {
    TEST_ASSERT(sockfd, "Server is not responding");
    TEST_ASSERT(close(sockfd) != -1, "Cannot close socket");
    return TEST_SUCCESS;
}

TEST(cleanup_db) {
    TEST_ASSERT(mpm_database_close(db) == 0, "Cannot close database");
    return TEST_SUCCESS;
}

void        begin_client_test(void) {
    reg_test("connect", connect_1);
    reg_test("connect", connect_db);
    reg_test("auth", pkg_auth_1_write);
    reg_test("auth", pkg_auth_1_read);
    reg_test("auth", pkg_auth_2_write);
    reg_test("auth", pkg_auth_1_read);
    reg_test("auth", pkg_auth_3_write);
    reg_test("auth", pkg_auth_1_read);
    reg_test("get_pkg", pkg_req_get_pkg_1_write);
    reg_test("get_pkg", pkg_req_get_pkg_1_read);
    reg_test("clean", cleanup_co);
    reg_test("clean", cleanup_db);
    test_all();
    test_free();
}
