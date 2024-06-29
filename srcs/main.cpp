# include "lib.hpp"

int     main(void) {
    if (!check_rights())
        exit_on_error("Error: Matt-Daemon must be started only with root privileges");
    try {
        unix_socket sock;
        // daemonize(true, true);
        // for (int i = 10; i >= 0; i--) {
        //     sleep(10);
        //     printf("%d\n", i);
        // }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 1;
}