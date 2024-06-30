# include "lib.hpp"

int     main(void) {
    if (!check_rights())
        exit_on_error("Error: Matt-Daemon must be started only with root privileges");
    try {
        unix_socket sock;
        lockfile    lock;
        // daemonize(true, true);
        sock.run();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 1;
}