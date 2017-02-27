#include "package.h"


static size_t                read_header(package_t *pkg, void *data) {
    size_t      count = 0;

    read_member(pkg->type);
    read_member(pkg->size);
    read_member(pkg->next_pkg_len);

    if (pkg->next_pkg_len != 0) {
        pkg->next_pkg = malloc(sizeof(char) * pkg->next_pkg_len);
        assert(pkg->next_pkg != NULL);
        read_string(pkg->next_pkg, pkg->next_pkg_len);
    } else {
        pkg->next_pkg = NULL;
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

    read_member(auth->mpm_major_version);
    read_member(auth->mpm_minor_version);

    list_add(pkg->payload, auth, sizeof(auth_t));
    return count;
}

static size_t       read_payload_auth_ack(package_t *pkg, void *data) {
    size_t      count = 0;
    auth_ack_t  *auth;

    auth = malloc(sizeof(auth_t));
    assert(auth != NULL);

    read_member(auth->mpm_major_version);
    read_member(auth->mpm_minor_version);

    list_add(pkg->payload, auth, sizeof(auth_ack_t));
    return count;
}

static size_t       read_payload_error(package_t *pkg, void *data) {
    size_t      count = 0;
    error_t     *err;

    err = malloc(sizeof(error_t));
    assert(err != NULL);

    read_member(err->error_type);
    read_member(err->error_len);

    if (err->error_len) {
        err->err = malloc(sizeof(char) * err->error_len);
        assert(err->err != NULL);
        read_string(err->err, err->error_len);
    } else {
        err->err = NULL;
    }

    list_add(pkg->payload, err, sizeof(error_t));
    return count;
}

static size_t       read_payload_get_package(package_t *pkg, void *data) {
    size_t      count = 0;
    req_get_pkg_t   *req;

    req = malloc(sizeof(req_get_pkg_t));
    assert(req != NULL);

    read_member(req->id);
    read_member(req->state);
    read_member(req->name_len);
    read_member(req->categ_len);
    read_member(req->version_len);

    if (req->name_len != 0) {
        req->name = malloc(sizeof(char) * req->name_len);
        assert(req->name != NULL);
        read_string(req->name, req->name_len);
    } else {
        req->name = NULL;
    }

    if (req->categ_len != 0) {
        req->category = malloc(sizeof(char) * req->categ_len);
        assert(req->category != NULL);
        read_string(req->category, req->categ_len);
    } else {
        req->category = NULL;
    }

    if (req->version_len != 0) {
        req->version = malloc(sizeof(char) * req->version_len);
        assert(req->version != NULL);
        read_string(req->version, req->version_len);
    } else {
        req->version = NULL;
    }

    list_add(pkg->payload, req, sizeof(req_get_pkg_t));
    return count;
}

static size_t           read_payload_get_file(package_t *pkg, void *data) {
    size_t      count = 0;
    req_get_file_t  *req;

    req = malloc(sizeof(req_get_file_t));
    assert(req != NULL);

    read_member(req->id);
    read_member(req->path_len);

    if (req->path_len != 0) {
        req->path = malloc(sizeof(char) * req->path_len);
        assert(req->path);
        read_string(req->path, req->path_len);
    } else {
        req->path = NULL;
    }

    list_add(pkg->payload, req, sizeof(req_get_file_t));
    return count;
}

static size_t           read_payload_get_news(package_t *pkg, void *data) {
    size_t      count = 0;
    req_get_news_t  *req;

    req = malloc(sizeof(req_get_news_t));
    assert(req != NULL);

    read_member(req->last_request);
    read_member(req->pkgs_ids_size);

    if (req->pkgs_ids_size != 0) {
        req->pkgs_ids = malloc(sizeof(*req->pkgs_ids) * req->pkgs_ids_size);
        assert(req->pkgs_ids);
        memcpy(req->pkgs_ids, data + count, sizeof(*req->pkgs_ids) * req->pkgs_ids_size);
        count += sizeof(*req->pkgs_ids) * req->pkgs_ids_size;
    } else {
        req->pkgs_ids = NULL;
    }

    list_add(pkg->payload, req, sizeof(req_get_news_t));
    return count;
}

static size_t           read_payload_get_cat(package_t *pkg, void *data) {
    size_t      count = 0;
    req_get_cat_t   *req;

    req = malloc(sizeof(req_get_cat_t));
    assert(req != NULL);

    read_member(req->cat_len);

    if (req->cat_len != 0) {
        req->categories = malloc(sizeof(*req->categories) * req->cat_len);
        assert(req->categories);
        memcpy(req->categories, data + count, sizeof(*req->categories) * req->cat_len);
        count += sizeof(*req->categories) * req->cat_len;
    } else {
        req->categories = NULL;
    }

    list_add(pkg->payload, req, sizeof(req_get_cat_t));
    return count;
}

static size_t           read_payload_get_upd(package_t *pkg, void *data) {
    size_t      count = 0;
    req_get_upd_t   *req;

    req = malloc(sizeof(sizeof(req_get_upd_t)));
    assert(req != NULL);

    read_member(req->pkg_len);

    if (req->pkg_len != 0) {
        req->packages = malloc(sizeof(*req->packages) * req->pkg_len);
        assert(req->packages);
        memcpy(req->packages, data + count, sizeof(*req->packages) * req->pkg_len);
        count += sizeof(*req->packages) * req->pkg_len;
    } else {
        req->packages = NULL;
    }

    list_add(pkg->payload, req, sizeof(req_get_upd_t));
    return count;
}

typedef     size_t      (*payload_callback)(package_t *, void *);
static const     payload_callback arr[] = {
    &read_payload_auth,
    &read_payload_auth_ack,
    &read_payload_error,
    &read_payload_get_package,
    &read_payload_get_file,
    &read_payload_get_news,
    &read_payload_get_cat,
    &read_payload_get_upd,
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
