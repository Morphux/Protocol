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


typedef     void      *(*write_callback)(void *, size_t *);
static const        write_callback arr[] = {
    &write_payload_auth
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

    header = write_header(pkg, &header_size);
    payload = write_payload(pkg, &payload_size);

    *size = header_size + payload_size;
    ret = malloc(*size);
    assert(ret != NULL);
    memcpy(ret, header, header_size);
    memcpy(ret + header_size, payload, payload_size);
    return ret;
}
