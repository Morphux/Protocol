#include "protocol_test.h"
#include "package.h"

int     main(int ac, char **av) {
    if (ac == 1)
        m_panic("Can't do without an argument\n");

    if (strcmp(av[1], "client") == 0) {
        /* Client */
    } else if (strcmp(av[1], "server") == 0) {
        /* Server */
    } else {
        m_panic("Unknown argument: %s\n", av[1]);
    }
    return 0;
}
