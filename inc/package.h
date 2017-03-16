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
    PKG_TYPE_AUTH = 1,
    PKG_TYPE_AUTH_ACK,
    PKG_TYPE_ERROR,
    PKG_TYPE_REQ_GET_PKG,
    PKG_TYPE_REQ_GET_FILE,
    PKG_TYPE_REQ_GET_NEWS,
    PKG_TYPE_REQ_CAT,
    PKG_TYPE_REQ_UPD,
    PKG_TYPE_RESP_PKG,
    PKG_TYPE_RESP_FILE,
    PKG_TYPE_RESP_NEWS,
    PKG_TYPE_RESP_CAT,
    PKG_TYPE_END
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

typedef struct      req_get_news_s {
    time_t      last_request;
    u16_t       pkgs_ids_size;
    u64_t       *pkgs_ids;
}         SF_PACKED req_get_news_t;

typedef struct      req_get_cat_s {
    u16_t       cat_len;
    u64_t       *categories;
}         SF_PACKED req_get_cat_t;

typedef struct      req_get_upd_s {
    u64_t       pkg_len;
    u64_t       *packages;
}         SF_PACKED req_get_upd_t;

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

typedef struct      resp_file_s {
    u64_t       id;
    u8_t        type;
    u64_t       parent_id;
    u16_t       path_len;
    char        *path;
}         SF_PACKED resp_file_t;

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

typedef struct      resp_cat_s {
    u64_t       id;
    u64_t       parent_id;
    u16_t       name_len;
    char        *name;
}         SF_PACKED resp_cat_t;

void        *write_package(package_t *pkg, size_t *size);
void *pkg_build_auth(size_t *size, int major_version, int minor_version);

#endif /* PACKAGE_H */
