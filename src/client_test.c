#include "client_test.h"

static int              sockfd;
static database_t       *db;

SQL_CALLBACK_DEF(pkg_cb) {
    mlist_t     **head = context;
    intern_package_t    *pkg;

    (void)context;
    pkg = malloc(sizeof(intern_package_t));
    assert(pkg != NULL);

    for (u8_t i = 0; i < col_num; i++)
    {
        if (strcmp(col_name[i], "id") == 0)
            pkg->id = strtoull(col_txt[i], (char **)NULL, 10);
        else if (strcmp(col_name[i], "name") == 0)
            pkg->name = strdup(col_txt[i]);
        else if (strcmp(col_name[i], "version") == 0)
            pkg->version = strdup(col_txt[i]);
        else if (strcmp(col_name[i], "category") == 0)
            pkg->category = strdup(col_txt[i]);
        else if (strcmp(col_name[i], "description") == 0)
            pkg->description = strdup(col_txt[i]);
        else if (strcmp(col_name[i], "archive") == 0)
            pkg->archive = strdup(col_txt[i]);
        else if (strcmp(col_name[i], "SBU") == 0)
            pkg->sbu = strtoull(col_txt[i], (char **)NULL, 10);
        else if (strcmp(col_name[i], "dependencies") == 0)
            pkg->dependencies = strdup(col_txt[i]);
        else if (strcmp(col_name[i], "archiveSize") == 0)
            pkg->arch_size = strtoull(col_txt[i], (char **)NULL, 10);
        else if (strcmp(col_name[i], "installedSize") == 0)
            pkg->inst_size = strtoull(col_txt[i], (char **)NULL, 10);
        else if (strcmp(col_name[i], "archiveHash") == 0)
            pkg->arch_hash = strdup(col_txt[i]);
        else if (strcmp(col_name[i], "state") == 0)
            pkg->state = PKG_STABLE;
        else if (strcmp(col_name[i], "timeAddPkg") == 0)
            ;
        else
        {
            printf("Unknown column: %s", col_name[i]);
            assert(0);
        }
    }

    char *tmp = pkg->dependencies, *token;
    u16_t       i = 0;
    while ((token = strtok_r(tmp, ", ", &tmp)) != NULL)
        i++;

    pkg->dependencies_arr = malloc(sizeof(u64_t) * i);
    tmp = pkg->dependencies;
    i = 0;

    while ((token = strtok_r(tmp, ", ", &tmp)) != NULL)
        pkg->dependencies_arr[i++] = strtoull(token, (char **)NULL, 10);

    pkg->dependencies_arr_size = i;
    list_add(*(head), pkg, sizeof(intern_package_t));
    free(pkg);
    return 0;
}

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
    u8_t        ret = 0;

    db = mpm_database_open(&ret, g_db_path);
    TEST_ASSERT(ret == 0, "Cannot open the database");
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
    prot_package_t   *pkg;
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
    prot_package_t   *pkg;
    size_t      r_n = 0, size;

    TEST_ASSERT(sockfd, "Server is not responding");
    expect = pkg_build_error(&size, ERR_MALFORMED_PACKET, "A packet send by the client is wrong");
    ret = malloc(2048);

    READ_TIMEOUT(sockfd, ret, 2048, 1, r_n);
    TEST_ASSERT_FMT(memcmp(ret, expect, size) == 0,
        "Expected package is wrong %s", print_package(expect, ret, size, r_n));
    pkg = read_pkg(ret);
    TEST_ASSERT(pkg, "Can't read package");
    TEST_ASSERT(pkg->type == PKG_TYPE_ERROR, "Package type is wrong");
    free(ret);
    return TEST_SUCCESS;
}

TEST(pkg_req_get_pkg_2_write) {
    void        *ret;
    size_t      size;

    TEST_ASSERT(sockfd, "Server is not responding");
    ret = pkg_build_req_get_pkg(&size, 1, PKG_STABLE, "", "", "");
    TEST_ASSERT(write(sockfd, ret, size), "Cannot send package to the server");
    return TEST_SUCCESS;
}


TEST(pkg_req_get_pkg_2_read) {
    mlist_t     *pkgs = NULL;
    intern_package_t   *pkg;
    package_t       *ptr;
    void        *ret = NULL;
    size_t      r_n = 0, size;
    char        *req;

    TEST_ASSERT(sockfd, "Server is not responding");

    ret = malloc(2048);
    asprintf(&req, "SELECT * FROM pkgs WHERE id = 1");
    mpm_database_exec(db, req, pkg_cb, &pkgs, &req);
    pkg = pkgs->member;
    READ_TIMEOUT(sockfd, ret, 2048, 1, r_n);

    ptr = pkg_build_resp_pkg(&size, pkg->id, pkg->sbu, pkg->inst_size,
        pkg->arch_size, pkg->state, pkg->name, pkg->category, pkg->version, pkg->description,
        pkg->archive, pkg->arch_hash, pkg->dependencies_arr_size,
        pkg->dependencies_arr);
    printf("\n%zu\n", size);
    TEST_ASSERT_FMT(memcmp(ptr, ret, size) == 0,
        "Expected package is wrong %s", print_package(ptr, ret, size, r_n));
    free(ret);
    return TEST_SUCCESS;
}

TEST(pkg_req_get_pkg_test_all) {
    mlist_t             *pkgs = NULL, *tmp;
    intern_package_t    *pkg;
    package_t           *ptr;
    void                *ret = NULL;
    size_t              r_n = 0, size;

    (void)r_n;
    TEST_ASSERT(sockfd, "Server is not responding");
    mpm_database_exec(db, "SELECT * FROM pkgs", pkg_cb, &pkgs, (char **)NULL);
    list_for_each(pkgs, tmp, pkg) {
        ret = pkg_build_req_get_pkg(&size, pkg->id, PKG_STABLE, "", "", "");
        TEST_ASSERT(write(sockfd, ret, size), "Cannot send package to the server");
        free(ret);
        ret = malloc(2048);
        READ_TIMEOUT(sockfd, ret, 2048, 1, r_n);
        ptr = pkg_build_resp_pkg(&size, pkg->id, pkg->sbu, pkg->inst_size,
            pkg->arch_size, pkg->state, pkg->name, pkg->category, pkg->version, pkg->description,
            pkg->archive, pkg->arch_hash, pkg->dependencies_arr_size,
            pkg->dependencies_arr);
        TEST_ASSERT_FMT(memcmp(ptr, ret, size) == 0,
            "Expected package is wrong %s", print_package(ptr, ret, size, r_n));
        free(ret);
        free(ptr);
    }
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
    reg_test("get_pkg", pkg_req_get_pkg_2_write);
    reg_test("get_pkg", pkg_req_get_pkg_2_read);
    reg_test("get_pkg", pkg_req_get_pkg_test_all);
    reg_test("clean", cleanup_co);
    reg_test("clean", cleanup_db);
    test_all();
    test_free();
}
