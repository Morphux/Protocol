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
    error_pkg_t *err;

    err = malloc(sizeof(error_pkg_t));
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

    list_add(pkg->payload, err, sizeof(error_pkg_t));
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

static size_t           read_payload_resp_pkg(package_t *pkg, void *data) {
    size_t      count = 0;
    resp_pkg_t      *resp;

    resp = malloc(sizeof(resp_pkg_t));
    assert(resp != NULL);

    read_member(resp->id);
    read_member(resp->comp_time);
    read_member(resp->inst_size);
    read_member(resp->arch_size);
    read_member(resp->state);
    read_member(resp->name_len);
    read_member(resp->category_len);
    read_member(resp->version_len);
    read_member(resp->archive_len);
    read_member(resp->checksum_len);
    read_member(resp->dependencies_size);

    if (resp->name_len != 0) {
        resp->name = malloc(sizeof(char) * resp->name_len);
        assert(resp->name);
        read_string(resp->name, resp->name_len);
    } else {
        resp->name = NULL;
    }

    if (resp->category_len != 0) {
        resp->category = malloc(sizeof(char *) * resp->category_len);
        assert(resp->category);
        read_string(resp->category, resp->category_len);
    } else {
        resp->category = NULL;
    }

    if (resp->version_len != 0) {
        resp->version = malloc(sizeof(char) * resp->version_len);
        assert(resp->version);
        read_string(resp->version, resp->version_len);
    } else {
        resp->version = NULL;
    }

    if (resp->archive_len != 0) {
        resp->archive = malloc(sizeof(char *) * resp->archive_len);
        assert(resp->archive);
        read_string(resp->archive, resp->archive_len);
    } else {
        resp->archive = NULL;
    }

    if (resp->checksum_len != 0) {
        resp->checksum = malloc(sizeof(char *) * resp->checksum_len);
        assert(resp->checksum);
        read_string(resp->checksum, resp->checksum_len);
    } else {
        resp->checksum = NULL;
    }

    if (resp->dependencies_size != 0) {
        resp->dependencies = malloc(sizeof(*resp->dependencies) * resp->dependencies_size);
        assert(resp->dependencies);
        memcpy(resp->dependencies, data + count, sizeof(*resp->dependencies) * resp->dependencies_size);
        count += sizeof(*resp->dependencies) * resp->dependencies_size;
    } else {
        resp->dependencies = NULL;
    }

    list_add(pkg->payload, resp, sizeof(resp_pkg_t));
    return count;
}

static size_t           read_payload_resp_file(package_t *pkg, void *data) {
    size_t      count = 0;
    resp_file_t *resp;

    resp = malloc(sizeof(resp_file_t));
    assert(resp != NULL);

    read_member(resp->id);
    read_member(resp->type);
    read_member(resp->parent_id);
    read_member(resp->path_len);

    if (resp->path_len != 0) {
        resp->path = malloc(sizeof(char) * resp->path_len);
        assert(resp->path != NULL);
        read_string(resp->path, resp->path_len);
    } else {
        resp->path = NULL;
    }

    list_add(pkg->payload, resp, sizeof(resp_file_t));
    return count;
}

static size_t           read_payload_resp_news(package_t *pkg, void *data) {
    size_t      count = 0;
    resp_news_t *resp;

    resp = malloc(sizeof(resp_news_t));
    assert(resp != NULL);

    read_member(resp->id);
    read_member(resp->parent_id);
    read_member(resp->author_len);
    read_member(resp->author_mail_len);
    read_member(resp->text_len);

    if (resp->author_len != 0) {
        resp->author = malloc(sizeof(char) * resp->author_len);
        assert(resp->author != NULL);
        read_string(resp->author, resp->author_len);
    } else {
        resp->author = NULL;
    }

    if (resp->author_mail_len != 0) {
        resp->author_mail = malloc(sizeof(char) * resp->author_mail_len);
        assert(resp->author_mail != NULL);
        read_string(resp->author_mail, resp->author_mail_len);
    } else {
        resp->author_mail = NULL;
    }

    if (resp->text_len != 0) {
        resp->text = malloc(sizeof(char *) * resp->text_len);
        assert(resp->text != NULL);
        read_string(resp->text, resp->text_len);
    } else {
        resp->text = NULL;
    }

    list_add(pkg->payload, resp, sizeof(resp_news_t));
    return count;
}

static size_t           read_payload_resp_cat(package_t *pkg, void *data) {
    size_t      count = 0;
    resp_cat_t  *resp;

    resp = malloc(sizeof(resp_cat_t));
    assert(resp != NULL);

    read_member(resp->id);
    read_member(resp->parent_id);
    read_member(resp->name_len);

    if (resp->name_len != 0) {
        resp->name = malloc(sizeof(char) * resp->name_len);
        assert(resp->name != NULL);
        read_string(resp->name, resp->name_len);
    } else {
        resp->name = NULL;
    }

    list_add(pkg->payload, resp, sizeof(resp_cat_t));
    return count;
}

typedef     size_t      (*payload_callback)(package_t *, void *);
typedef     struct callback_s {
    u8_t               index;
    payload_callback   fn;
} callback_t;

static const        callback_t arr[] = {
    {PKG_TYPE_AUTH, &read_payload_auth},
    {PKG_TYPE_AUTH_ACK, &read_payload_auth_ack},
    {PKG_TYPE_ERROR, &read_payload_error},
    {PKG_TYPE_REQ_GET_PKG, &read_payload_get_package},
    {PKG_TYPE_REQ_GET_FILE, &read_payload_get_file},
    {PKG_TYPE_REQ_GET_NEWS, &read_payload_get_news},
    {PKG_TYPE_REQ_CAT, &read_payload_get_cat},
    {PKG_TYPE_REQ_UPD, &read_payload_get_upd},
    {PKG_TYPE_RESP_PKG, &read_payload_resp_pkg},
    {PKG_TYPE_RESP_FILE, &read_payload_resp_file},
    {PKG_TYPE_RESP_NEWS, &read_payload_resp_news},
    {PKG_TYPE_RESP_CAT, &read_payload_resp_cat}
};

static size_t   read_payload(package_t *pkg, void *data) {
    size_t      index;

    for (index = 0; index < sizeof(arr) / sizeof(arr[0]); index++)
    {
        if (pkg->type == arr[index].index)
            return arr[index].fn(pkg, data);
    }
    return 0;
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
