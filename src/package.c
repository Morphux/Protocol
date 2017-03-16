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
    pkg->number = 1;
    list_add(pkg->payload, auth, sizeof(auth_t));
    return write_package(pkg, size);
}
