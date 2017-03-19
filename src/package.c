# include "package.h"

void *pkg_build_auth(size_t *size, int major_version, int minor_version) {
    auth_t      *auth;
    package_t   *pkg;

    auth = malloc(sizeof(auth_t));
    auth->mpm_major_version = major_version;
    auth->mpm_minor_version = minor_version;
    pkg = malloc(sizeof(package_t));
    pkg->type = PKG_TYPE_AUTH;
    pkg->payload = NULL;
    list_add(pkg->payload, auth, sizeof(auth_t));
    return write_package(pkg, size);
}

void *pkg_build_auth_ack(size_t *size, int major_version, int minor_version) {
    auth_t      *auth;
    package_t   *pkg;
    void        *ret;

    auth = malloc(sizeof(auth_t));
    auth->mpm_major_version = major_version;
    auth->mpm_minor_version = minor_version;
    pkg = malloc(sizeof(package_t));
    pkg->type = PKG_TYPE_AUTH_ACK;
    pkg->payload = NULL;
    list_add(pkg->payload, auth, sizeof(auth_t));
    ret = write_package(pkg, size);
    return ret;
}

void *pkg_build_error(size_t *size, error_type_t type, const char *error) {
    error_pkg_t     *err;
    package_t       *pkg;

    err = malloc(sizeof(error_pkg_t));
    err->error_type = type;
    err->error_len = strlen(error);
    err->err = strdup(error);

    pkg = malloc(sizeof(package_t));
    pkg->type = PKG_TYPE_ERROR;
    pkg->payload = NULL;
    list_add(pkg->payload, err, SIZEOF_ERR(err));
    return write_package(pkg, size);
}

void *pkg_build_req_get_pkg(size_t *size, u64_t id, u8_t state, const char *name, 
            const char *category, const char *version) {
    req_get_pkg_t       *req;
    package_t           *pkg;

    req = malloc(sizeof(req_get_pkg_t));
    req->id = id;
    req->state = state;
    req->name_len = strlen(name);
    req->categ_len = strlen(category);
    req->version_len = strlen(version);
    req->name = strdup(name);
    req->category = strdup(category);
    req->version = strdup(version);

    pkg = malloc(sizeof(package_t));
    pkg->type = PKG_TYPE_REQ_GET_PKG;
    pkg->payload = NULL;
    list_add(pkg->payload, req, SIZEOF_REQ_GET_PKG(req));
    return write_package(pkg, size);
}

void *pkg_build_req_get_file(size_t *size, u64_t id, const char *path) {
    req_get_file_t      *file;
    package_t           *pkg;

    file = malloc(sizeof(req_get_file_t));
    file->id = id;
    file->path_len = strlen(path);
    file->path = strdup(path);

    pkg = malloc(sizeof(package_t));
    pkg->type = PKG_TYPE_REQ_GET_FILE;
    pkg->payload = NULL;
    list_add(pkg->payload, file, SIZEOF_REQ_GET_FILE(file));
    return write_package(pkg, size);
}

void *pkg_build_req_get_news(size_t *size, time_t last_request, u16_t ids_size, 
                                u64_t *ids) {
    req_get_news_t      *news;
    package_t           *pkg;

    news = malloc(sizeof(req_get_news_t));
    news->last_request = last_request;
    news->pkgs_ids_size = ids_size;
    news->pkgs_ids = malloc(sizeof(*ids) * ids_size);
    memcpy(news->pkgs_ids, ids, sizeof(*ids) * ids_size);

    pkg = malloc(sizeof(package_t));
    pkg->type = PKG_TYPE_REQ_GET_NEWS;
    pkg->payload = NULL;
    list_add(pkg->payload, news, SIZEOF_REQ_GET_NEWS(news));
    return write_package(pkg, size);
}

void *pkg_build_req_get_cat(size_t *size, u16_t len, u64_t *a_cat) {
    req_get_cat_t       *cat;
    package_t           *pkg;

    cat = malloc(sizeof(req_get_cat_t));
    cat->cat_len = len;
    cat->categories = malloc(sizeof(*a_cat) * len);
    memcpy(cat->categories, a_cat, sizeof(*a_cat) * len);

    pkg = malloc(sizeof(package_t));
    pkg->type = PKG_TYPE_REQ_CAT;
    pkg->payload = NULL;
    list_add(pkg->payload, cat, SIZEOF_REQ_GET_CAT(cat));
    return write_package(pkg, size);
}

void *pkg_build_req_get_upd(size_t *size, u64_t len, u64_t *a_pkgs) {
    req_get_upd_t       *upd;
    package_t           *pkg;

    upd = malloc(sizeof(req_get_upd_t));
    upd->pkg_len = len;
    upd->packages = malloc(sizeof(*a_pkgs) * len);
    memcpy(upd->packages, a_pkgs, sizeof(*a_pkgs) * len);

    pkg = malloc(sizeof(package_t));
    pkg->type = PKG_TYPE_REQ_UPD;
    pkg->payload = NULL;
    list_add(pkg->payload, upd, SIZEOF_REQ_GET_UPD(upd));
    return write_package(pkg, size);
}
