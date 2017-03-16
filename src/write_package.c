#include "package.h"

static void *write_header(package_t *pkg, size_t *count) {
    void    *header;

    *count = 0;
    header = malloc(sizeof(pkg->type) + 
                    sizeof(pkg->size) + 
                    sizeof(pkg->next_pkg_len) +
                    sizeof(pkg->number) +
                    pkg->next_pkg_len);
    assert(header != NULL);

    write_member(pkg->type, header, *count);
    write_member(pkg->size, header, *count);
    write_member(pkg->next_pkg_len, header, *count);
    if (pkg->next_pkg_len != 0) {
        write_string(pkg->next_pkg, header, pkg->next_pkg_len, *count);
    }
    write_member(pkg->number, header, *count);
    return header;
}

static void     *write_payload_auth(void *pkg, size_t *count) {
    auth_t      *auth = pkg;
    void        *ret;

    ret = malloc(sizeof(auth_t));
    assert(ret != NULL);

    write_member(auth->mpm_major_version, ret, *count);
    write_member(auth->mpm_minor_version, ret, *count);
    return ret;
}

static void     *write_payload_auth_ack(void *pkg, size_t *count) {
    auth_ack_t      *auth = pkg;
    void            *ret;

    ret = malloc(sizeof(auth_ack_t));
    assert(ret != NULL);

    write_member(auth->mpm_major_version, ret, *count);
    write_member(auth->mpm_minor_version, ret, *count);
    return ret;
}

static void     *write_payload_error(void *pkg, size_t *count) {
    error_pkg_t *err = pkg;
    void        *ret;

    ret = malloc(sizeof(err->error_type) + sizeof(err->error_len) + err->error_len);
    assert(ret != NULL);

    write_member(err->error_type, ret, *count);
    write_member(err->error_len, ret, *count);
    write_string(err->err, ret, err->error_len, *count);

    return ret;
}

static void     *write_payload_req_get_pkg(void *pkg, size_t *count) {
    req_get_pkg_t   *req = pkg;
    void            *ret;

    ret = malloc(sizeof(req->id) + sizeof(req->state) + sizeof(req->name_len)
            + sizeof(req->categ_len) + sizeof(req->version_len)
            + req->name_len + req->categ_len + req->version_len);
    assert(req != NULL);

    write_member(req->id, ret, *count);
    write_member(req->state, ret, *count);
    write_member(req->name_len, ret, *count);
    write_member(req->categ_len, ret, *count);
    write_member(req->version_len, ret, *count);
    write_string(req->name, ret, req->name_len, *count);
    write_string(req->category, ret, req->categ_len, *count);
    write_string(req->version, ret, req->version_len, *count);

    return ret;
}

static void     *write_payload_req_get_file(void *pkg, size_t *count) {
    req_get_file_t      *req = pkg;
    void                *ret;

    ret = malloc(sizeof(req->id) + sizeof(req->path_len) + req->path_len);
    assert(ret != NULL);

    write_member(req->id, ret, *count);
    write_member(req->path_len, ret, *count);
    write_string(req->path, ret, req->path_len, *count);

    return ret;
}

static void     *write_payload_req_get_news(void *pkg, size_t *count) {
    req_get_news_t      *req = pkg;
    void                *ret;

    ret = malloc(sizeof(req->last_request) + sizeof(req->pkgs_ids_size) +
        (req->pkgs_ids_size * sizeof(*req->pkgs_ids)));
    assert(ret != NULL);

    write_member(req->last_request, ret, *count);
    write_member(req->pkgs_ids_size, ret, *count);
    for (u16_t i = 0; i < req->pkgs_ids_size; i++) {
        memcpy(ret + *count, &(req->pkgs_ids[i]), sizeof(*req->pkgs_ids));
        *count += sizeof(*req->pkgs_ids);
    }

    return ret;
}

static void     *write_payload_req_get_cat(void *pkg, size_t *count) {
    req_get_cat_t       *req = pkg;
    void                *ret;

    ret = malloc(sizeof(req->cat_len) + (sizeof(*req->categories) * req->cat_len));
    assert(ret != NULL);

    write_member(req->cat_len, ret, *count);
    for (u16_t i = 0; i < req->cat_len; i++) {
        memcpy(ret + *count, &(req->categories[i]), sizeof(*req->categories));
        *count += sizeof(*req->categories);
    }
    return ret;
}

static void     *write_payload_req_get_upd(void *pkg, size_t *count) {
    req_get_upd_t       *req = pkg;
    void                *ret;

    ret = malloc(sizeof(req->pkg_len) + (sizeof(*req->packages) * req->pkg_len));
    assert(ret != NULL);

    write_member(req->pkg_len, ret, *count);
    for (u64_t i = 0; i < req->pkg_len; i++) {
        memcpy(ret + *count, &(req->packages[i]), sizeof(*req->packages));
        *count += sizeof(*req->packages);
    }
    return ret;
}

static void     *write_payload_resp_pkg(void *pkg, size_t *count) {
    resp_pkg_t      *resp = pkg;
    void            *ret;

    ret = malloc(sizeof(resp->id) +
            sizeof(resp->comp_time) +
            sizeof(resp->inst_size) +
            sizeof(resp->arch_size) +
            sizeof(resp->state) +
            sizeof(resp->name_len) +
            sizeof(resp->category_len) +
            sizeof(resp->version_len) +
            sizeof(resp->archive_len) +
            sizeof(resp->checksum_len) +
            sizeof(resp->dependencies_size) +
            resp->name_len +
            resp->category_len +
            resp->version_len +
            resp->archive_len +
            resp->checksum_len +
            sizeof(resp->dependencies_size) * sizeof(*resp->dependencies)
        );
    assert(ret != NULL);
    write_member(resp->id, ret, *count);
    write_member(resp->comp_time, ret, *count);
    write_member(resp->inst_size, ret, *count);
    write_member(resp->arch_size, ret, *count);
    write_member(resp->state, ret, *count);
    write_member(resp->name_len, ret, *count);
    write_member(resp->category_len, ret, *count);
    write_member(resp->version_len, ret, *count);
    write_member(resp->archive_len, ret, *count);
    write_member(resp->checksum_len, ret, *count);
    write_member(resp->dependencies_size, ret, *count);

    write_string(resp->name, ret, resp->name_len, *count);
    write_string(resp->category, ret, resp->category_len, *count);
    write_string(resp->version, ret, resp->version_len, *count);
    write_string(resp->archive, ret, resp->archive_len, *count);
    write_string(resp->checksum, ret, resp->checksum_len, *count);

    for (u16_t i = 0; i < resp->dependencies_size; i++) {
        memcpy(ret + *count, &resp->dependencies[i], sizeof(resp->dependencies[i]));
        *count += sizeof(resp->dependencies[i]);
    }

    return ret;
}

static void     *write_payload_resp_file(void *pkg, size_t *count) {
    resp_file_t     *resp = pkg;
    void            *ret;

    ret = malloc(sizeof(resp->id) + sizeof(resp->type) + sizeof(resp->parent_id) + 
            sizeof(resp->path_len) + resp->path_len);
    assert(pkg != ret);

    write_member(resp->id, ret, *count);
    write_member(resp->type, ret, *count);
    write_member(resp->parent_id, ret, *count);
    write_member(resp->path_len, ret, *count);
    write_string(resp->path, ret, resp->path_len, *count);

    return ret;
}

static void     *write_payload_resp_news(void *pkg, size_t *count) {
    resp_news_t     *resp = pkg;
    void            *ret;

    ret = malloc(sizeof(resp->id) + sizeof(resp->parent_id) + sizeof(resp->author_len) +
        sizeof(resp->author_mail_len) + sizeof(resp->text_len) +
        resp->author_len + resp->author_mail_len + resp->text_len);
    assert(ret != NULL);

    write_member(resp->id, ret, *count);
    write_member(resp->parent_id, ret, *count);
    write_member(resp->author_len, ret, *count);
    write_member(resp->author_mail_len, ret, *count);
    write_member(resp->text_len, ret, *count);
    write_string(resp->author, ret, resp->author_len, *count);
    write_string(resp->author_mail, ret, resp->author_mail_len, *count);
    write_string(resp->text, ret, resp->text_len, *count);

    return ret;
}

static void     *write_payload_resp_cat(void *pkg, size_t *count) {
    resp_cat_t      *resp = pkg;
    void            *ret;

    ret = malloc(sizeof(resp->id) + sizeof(resp->parent_id) + sizeof(resp->name_len)
            + resp->name_len);
    assert(ret != NULL);

    write_member(resp->id, ret, *count);
    write_member(resp->parent_id, ret, *count);
    write_member(resp->name_len, ret, *count);
    write_string(resp->name, ret, resp->name_len, *count);

    return ret;
}

typedef     void      *(*write_callback)(void *, size_t *);
static const        write_callback arr[] = {
    &write_payload_auth,
    &write_payload_auth_ack,
    &write_payload_error,
    &write_payload_req_get_pkg,
    &write_payload_req_get_file,
    &write_payload_req_get_news,
    &write_payload_req_get_cat,
    &write_payload_req_get_upd,
    &write_payload_resp_pkg,
    &write_payload_resp_file,
    &write_payload_resp_news,
    &write_payload_resp_cat
};

void        *write_payload(package_t *pkg, size_t *count) {
    mlist_t *tmp;
    void    *ptr, *payl_tmp, *ret = NULL;
    size_t  size;

    list_for_each(pkg->payload, tmp, ptr) {
        size = 0;
        payl_tmp = arr[pkg->type](ptr, &size);
        *count += size;
        ret = realloc(ret, *count);
        memcpy(ret + (*count - size), payl_tmp, size);
    }
    return ret;
}

void        *write_package(package_t *pkg, size_t *size) {
    void    *header, *payload, *ret;
    size_t  header_size = 0, payload_size = 0;

    pkg->size = 0;
    pkg->next_pkg_len = 0;
    header = write_header(pkg, &header_size);
    payload = write_payload(pkg, &payload_size);

    *size = header_size + payload_size;
    ret = malloc(*size);
    assert(ret != NULL);
    memcpy(ret, header, header_size);
    memcpy(ret + header_size, payload, payload_size);
    return ret;
}
