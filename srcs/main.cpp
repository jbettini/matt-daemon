# include "lib.hpp"

int     main(void) {
    if (!check_rights())
        exit_on_error("Error: Matt-Daemon must be started only with root privileges");
    Tintin_reporter reporter;
    daemonize(true, true);
    // reporter.save_logs()
    return 1;
}