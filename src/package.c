#include "package.h"


static size_t                read_header(package_t *pkg, void *data) {
    size_t      count = 0;

    memcpy(&pkg->type, data + count, sizeof(pkg->type));
    count += sizeof(pkg->type);
    memcpy(&pkg->size, data + count, sizeof(pkg->size));
    count += sizeof(pkg->size);
    memcpy(&pkg->next_pkg_len, data + count, sizeof(pkg->next_pkg_len));
    count += sizeof(pkg->next_pkg_len);

    if (pkg->next_pkg_len != 0) {
        pkg->next_pkg = malloc(sizeof(char) * pkg->next_pkg_len);
        memcpy(pkg->next_pkg, data + count, pkg->next_pkg_len);
        count += pkg->next_pkg_len;
    }
    return count;
}

static bool     check_type(package_t *pkg) {
    if (pkg->type >= PKG_TYPE_END)
        return false;
    return true;
}

static size_t       read_payload_auth(package_t *pkg, void *data) {
    size_t      count = 0;
    auth_t      *auth;

    auth = malloc(sizeof(auth_t));
    assert(auth != NULL);
    memcpy(&auth->mpm_major_version, data + count, sizeof(auth->mpm_major_version));
    count += sizeof(auth->mpm_major_version);
    memcpy(&auth->mpm_minor_version, data + count, sizeof(auth->mpm_minor_version));
    count += sizeof(auth->mpm_minor_version);
    list_add(pkg->payload, auth, sizeof(auth_t));
    return count;
}

static size_t       read_payload_auth_ack(package_t *pkg, void *data) {
    size_t      count = 0;
    auth_ack_t  *auth;

    auth = malloc(sizeof(auth_t));
    assert(auth != NULL);
    memcpy(&auth->mpm_major_version, data + count, sizeof(auth->mpm_major_version));
    count += sizeof(auth->mpm_major_version);
    memcpy(&auth->mpm_minor_version, data + count, sizeof(auth->mpm_minor_version));
    count += sizeof(auth->mpm_minor_version);
    list_add(pkg->payload, auth, sizeof(auth_ack_t));
    return count;
}

static size_t       read_payload_error(package_t *pkg, void *data) {
    size_t      count = 0;
    error_t     *err;

    err = malloc(sizeof(error_t));
    assert(err != NULL);
    memcpy(&err->error_type, data + count, sizeof(err->error_type));
    count += sizeof(err->error_type);
    memcpy(&err->error_len, data + count, sizeof(err->error_len));
    count += sizeof(err->error_len);

    err->err = malloc(sizeof(char) * err->error_len);
    assert(err->err != NULL);
    memcpy(err->err, data + count, err->error_len);
    count += err->error_len;
    list_add(pkg->payload, err, sizeof(error_t));
    return count;
}

static size_t       read_payload_get_package(package_t *pkg, void *data) {
    size_t      count = 0;
    req_get_pkg_t   *req;

    req = malloc(sizeof(req_get_pkg_t));
    assert(req != NULL);

    memcpy(&req->id, data + count, sizeof(req->id));
    count += sizeof(req->id);
    memcpy(&req->state, data + count, sizeof(req->state));
    count += sizeof(req->state);
    memcpy(&req->name_len, data + count, sizeof(req->name_len));
    count += sizeof(req->name_len);
    memcpy(&req->categ_len, data + count, sizeof(req->categ_len));
    count += sizeof(req->categ_len);
    memcpy(&req->version_len, data + count, sizeof(req->version_len));
    count += sizeof(req->version_len);

    memcpy(req->name, data + count, req->name_len);
    count += req->name_len;
    memcpy(req->category, data + count, req->categ_len);
    count += req->categ_len;
    memcpy(req->version, data + count, req->version_len);
    count += req->version_len;

    list_add(pkg->payload, req, sizeof(req_get_pkg_t));
    return count;
}

typedef     size_t      (*payload_callback)(package_t *, void *);
static const     payload_callback arr[] = {
    &read_payload_auth,
    &read_payload_auth_ack,
    &read_payload_error,
    &read_payload_get_package,
    NULL
};

static size_t   read_payload(package_t *pkg, void *data) {
    return arr[pkg->type](pkg, data);
}

package_t      *read_pkg(void *data) {
    package_t      *pkg;
    size_t          count;
    u8_t            number;

    pkg = malloc(sizeof(package_t));
    assert(pkg != NULL);
    pkg->payload = NULL;
    count = read_header(pkg, data);

    if (!check_type(pkg))
        return NULL;

    data += count;
    memcpy(&number, data, sizeof(u8_t));
    data += sizeof(u8_t);
    while (number) {
        data += read_payload(pkg, data);
        number--;
    }

    return pkg;
}
