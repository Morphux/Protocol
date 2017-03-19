#ifndef PACKAGE_H
# define PACKAGE_H

# include <morphux.h>
# include "protocol_test.h"

# define read_member(mem) memcpy(&(mem), data + count, sizeof(mem)); \
                            count += sizeof(mem);
# define read_string(mem, len) memcpy(mem, data + count, len); \
                            count += len;

# define write_member(mem, data, count) memcpy(data + count, &mem, sizeof(mem)); \
                            count += sizeof(mem);

# define write_string(mem, data, len, count) if (mem != NULL) { \
                                memcpy(data + count, mem, len); \
                                count += len; \
                                }

/* Header */
typedef struct      package_s {
    u8_t        type;
    u16_t       size;
    u8_t        next_pkg_len;
    char        *next_pkg;
    u8_t        number;
    mlist_t     *payload;
}        SF_PACKED package_t;

/* Type field of the header */
typedef enum        package_type_e {
    PKG_TYPE_AUTH = 0x1,
    PKG_TYPE_AUTH_ACK = 0x2,
    PKG_TYPE_ERROR = 0x3,
    PKG_TYPE_REQ_GET_PKG = 0x10,
    PKG_TYPE_REQ_GET_FILE = 0x11,
    PKG_TYPE_REQ_GET_NEWS = 0x12,
    PKG_TYPE_REQ_CAT = 0x13,
    PKG_TYPE_REQ_UPD = 0x14,
    PKG_TYPE_RESP_PKG = 0x20,
    PKG_TYPE_RESP_FILE = 0x21,
    PKG_TYPE_RESP_NEWS = 0x22,
    PKG_TYPE_RESP_CAT = 0x23,
    PKG_TYPE_END = 0x24
}                   package_type_t;

/* Payloads */

typedef struct      auth_s {
    u8_t        mpm_major_version;
    u8_t        mpm_minor_version;
}          SF_PACKED auth_t;

typedef struct       auth_ack_s {
    u8_t        mpm_major_version;
    u8_t        mpm_minor_version;
}          SF_PACKED auth_ack_t;

typedef struct      error_pkg_s {
    u8_t        error_type;
    u16_t       error_len;
    char        *err;
}          SF_PACKED error_pkg_t;

# define SIZEOF_ERR(err) (sizeof(err->error_type) + sizeof(err->error_len) \
                            + err->error_len)

/* Error possible types */
typedef enum        error_type_e {
    ERR_SERVER_FAULT = 1,
    ERR_MALFORMED_PACKET,
    ERR_RES_NOT_FOUND,
    ERR_END
}         SF_PACKED error_type_t;

typedef struct      req_get_pkg_s {
    u64_t       id;
    u8_t        state;
    u16_t       name_len;
    u16_t       categ_len;
    u16_t       version_len;
    char        *name;
    char        *category;
    char        *version;
}          SF_PACKED req_get_pkg_t;

# define SIZEOF_REQ_GET_PKG(pkg) (sizeof(pkg->id) + sizeof(pkg->state) + \
                                    sizeof(pkg->name_len) + sizeof(pkg->categ_len) + \
                                    sizeof(pkg->version_len) + pkg->name_len + pkg->categ_len + \
                                    pkg->version_len)

/* Stability of packages */
typedef enum        package_state_e {
    PKG_STABLE = 1,
    PKG_UNSTABLE,
    PKG_DEV,
    PKG_END
}                   package_state_t;

typedef struct      req_get_file_s {
    u64_t       id;
    u16_t       path_len;
    char        *path;
}         SF_PACKED req_get_file_t;

# define SIZEOF_REQ_GET_FILE(file) (sizeof(file->id) + sizeof(file->path_len) + file->path_len)

typedef struct      req_get_news_s {
    time_t      last_request;
    u16_t       pkgs_ids_size;
    u64_t       *pkgs_ids;
}         SF_PACKED req_get_news_t;

# define SIZEOF_REQ_GET_NEWS(news) (sizeof(news->last_request) + sizeof(news->pkgs_ids_size) + \
                                    sizeof(*news->pkgs_ids) * news->pkgs_ids_size)

typedef struct      req_get_cat_s {
    u16_t       cat_len;
    u64_t       *categories;
}         SF_PACKED req_get_cat_t;

# define SIZEOF_REQ_GET_CAT(cat) (sizeof(cat->cat_len) + sizeof(*cat->categories) * cat->cat_len)

typedef struct      req_get_upd_s {
    u64_t       pkg_len;
    u64_t       *packages;
}         SF_PACKED req_get_upd_t;

# define SIZEOF_REQ_GET_UPD(upd) (sizeof(upd->pkg_len) + sizeof(*upd->packages) * upd->pkg_len)

typedef struct      resp_pkg_s {
    u64_t       id;
    float       comp_time;
    float       inst_size;
    float       arch_size;
    u8_t        state;
    u16_t       name_len;
    u16_t       category_len;
    u16_t       version_len;
    u16_t       archive_len;
    u16_t       checksum_len;
    u16_t       dependencies_size;
    char        *name;
    char        *category;
    char        *version;
    char        *archive;
    char        *checksum;
    u64_t       *dependencies;
}       SF_PACKED resp_pkg_t;

# define SIZEOF_RESP_PKG(pkg) (sizeof(pkg->id) + sizeof(pkg->comp_time) + sizeof(pkg->inst_size) + \
                                sizeof(pkg->arch_size) + sizeof(pkg->state) + sizeof(pkg->name_len) + \
                                sizeof(pkg->category_len) + sizeof(pkg->version_len) + \
                                sizeof(pkg->archive_len) + sizeof(pkg->checksum_len) + \
                                sizeof(pkg->dependencies_size) + pkg->name_len + \
                                pkg->category_len + pkg->version_len + pkg->archive_len + \
                                pkg->checksum_len + (sizeof(*pkg->dependencies) * pkg->dependencies_size))

typedef struct      resp_file_s {
    u64_t       id;
    u8_t        type;
    u64_t       parent_id;
    u16_t       path_len;
    char        *path;
}         SF_PACKED resp_file_t;

# define SIZEOF_RESP_FILE(f) (sizeof(f->id) + sizeof(f->type) + sizeof(f->parent_id) + \
                                sizeof(f->path_len) + f->path_len)

typedef struct      resp_news_s {
    u64_t       id;
    u64_t       parent_id;
    u16_t       author_len;
    u16_t       author_mail_len;
    u16_t       text_len;
    char        *author;
    char        *author_mail;
    char        *text;
}          SF_PACKED resp_news_t;

# define SIZEOF_RESP_NEWS(n) (sizeof(n->id) + sizeof(n->parent_id) + sizeof(n->author_len) + \
                                sizeof(n->author_mail_len) + sizeof(n->text_len) + \
                                n->author_len + n->author_mail_len + n->text_len)

typedef struct      resp_cat_s {
    u64_t       id;
    u64_t       parent_id;
    u16_t       name_len;
    char        *name;
}         SF_PACKED resp_cat_t;

void        *write_package(package_t *pkg, size_t *size);
package_t      *read_pkg(void *data);
void *pkg_build_auth(size_t *size, int major_version, int minor_version);
void *pkg_build_auth_ack(size_t *size, int major_version, int minor_version);
void *pkg_build_error(size_t *size, error_type_t type, const char *error);
void *pkg_build_req_get_pkg(size_t *size, u64_t id, u8_t state, const char *name, 
            const char *category, const char *version);
void *pkg_build_req_get_file(size_t *size, u64_t id, const char *path);
void *pkg_build_req_get_news(size_t *size, time_t last_request, u16_t ids_size, 
                                u64_t *ids);
void *pkg_build_req_get_cat(size_t *size, u16_t len, u64_t *a_cat);
void *pkg_build_req_get_upd(size_t *size, u64_t len, u64_t *a_pkgs);
void *pkg_build_resp_pkg(size_t *size, u64_t id, float comp_time,
        float inst_size, float arch_size, u8_t state, const char *name, 
        const char *category, const char *version, const char *archive,
        const char *checksum, u16_t dep_size, u64_t *dependencies);
void *pkg_build_resp_file(size_t *size, u64_t id, u8_t type, u64_t parent_id,
                            const char *path);
void *pkg_build_resp_news(size_t *size, u64_t id, u64_t parent_id, const char *author,
                            const char *author_mail, const char *text);

#endif /* PACKAGE_H */
