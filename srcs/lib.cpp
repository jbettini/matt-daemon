# include "lib.hpp"

void exit_on_error(const char *err) {
    std::cout << err << std::endl;
    exit(EXIT_FAILURE);
}

bool    check_rights() {
    return (!getuid() && !geteuid());
}

void    redir_to_devnull() {
    int devnull;
    if ((devnull = open("/dev/null", O_WRONLY)) == -1)
        exit_on_error("Error: /dev/null failed to open");

    if (dup2(devnull, STDIN_FILENO) == -1 || \
            dup2(devnull, STDOUT_FILENO) == -1 || \
                dup2(devnull, STDERR_FILENO) == -1) {
        close(devnull);
        exit_on_error("Error: /dev/null failed to dup2");
    }
    close(devnull);
}

// TODO: return PID
void    daemonize(bool nochdir, bool noclose) {
    pid_t   pid;
    pid = fork();
    if (pid < 0)
        exit_on_error("Error: fork failed");
    else if (pid > 0)
        exit(EXIT_SUCCESS);
    if (setsid() < 0)
        exit_on_error("Error: setsid failed");
    /* TODO: Implement a working signal handler */
    // signal(SIGCHLD, SIG_IGN);
    // signal(SIGHUP, SIG_IGN);
    pid = fork();
    if (pid < 0)
        exit_on_error("Error: fork failed");
    if (pid > 0)
        exit(EXIT_SUCCESS);
    /* TODO: Implement umask */
    // umask(777);
    if (!nochdir)
        chdir("/");
    for (int x = sysconf(_SC_OPEN_MAX); x >= 3; x--) {
        close(x);
    }
    if (!noclose)
        redir_to_devnull();
}