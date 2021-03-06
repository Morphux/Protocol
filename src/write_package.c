#include "package.h"

static float reverse_float(const float in_float) {
   float ret_val;
   char *floatToConvert = (char *)&in_float;
   char *returnFloat = (char *)&ret_val;

   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return ret_val;
}


static void *write_header(prot_package_t *pkg, size_t *count) {
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
    assert(ret != NULL);

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
            sizeof(resp->dependencies_size) * sizeof(*resp->dependencies) + 100
        );
    assert(ret != NULL);
    resp->comp_time = reverse_float(resp->comp_time);
    resp->inst_size = reverse_float(resp->inst_size);
    resp->arch_size = reverse_float(resp->arch_size);
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
        *count += sizeof(u64_t);
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
typedef     struct callback_s {
    u8_t             index;
    write_callback   fn;
} callback_t;

static const        callback_t arr[] = {
    {PKG_TYPE_AUTH, &write_payload_auth},
    {PKG_TYPE_AUTH_ACK, &write_payload_auth_ack},
    {PKG_TYPE_ERROR, &write_payload_error},
    {PKG_TYPE_REQ_GET_PKG, &write_payload_req_get_pkg},
    {PKG_TYPE_REQ_GET_FILE, &write_payload_req_get_file},
    {PKG_TYPE_REQ_GET_NEWS, &write_payload_req_get_news},
    {PKG_TYPE_REQ_CAT, &write_payload_req_get_cat},
    {PKG_TYPE_REQ_UPD, &write_payload_req_get_upd},
    {PKG_TYPE_RESP_PKG, &write_payload_resp_pkg},
    {PKG_TYPE_RESP_FILE, &write_payload_resp_file},
    {PKG_TYPE_RESP_NEWS, &write_payload_resp_news},
    {PKG_TYPE_RESP_CAT, &write_payload_resp_cat}
};

void        *write_payload(prot_package_t *pkg, size_t *count) {
    mlist_t *tmp;
    void    *ptr, *payl_tmp, *ret = NULL;
    size_t  size, index;

    list_for_each(pkg->payload, tmp, ptr) {
        size = 0;
        for (index = 0; index < sizeof(arr) / sizeof(arr[0]); index++)
        {
            if (pkg->type == arr[index].index)
            {
                payl_tmp = arr[index].fn(ptr, &size);
                break ;
            }
        }
        assert(index != sizeof(arr) / sizeof(arr[0]));
        *count += size;
        ret = realloc(ret, *count);
        memcpy(ret + (*count - size), payl_tmp, size);
    }
    return ret;
}

void        *write_package(prot_package_t *pkg, size_t *size) {
    void    *header, *payload, *ret;
    size_t  header_size = 0, payload_size = 0;

    pkg->size = 0;
    pkg->next_pkg_len = 0;
    pkg->number = list_size(pkg->payload);
    header = write_header(pkg, &header_size);
    payload = write_payload(pkg, &payload_size);

    *size = header_size + payload_size;
    ret = malloc(*size);
    u16_t       size_tmp = (u16_t)*size;
    memcpy(header + 1, &size_tmp, sizeof(size_tmp));
    assert(ret != NULL);
    memcpy(ret, header, header_size);
    memcpy(ret + header_size, payload, payload_size);
    return ret;
}
