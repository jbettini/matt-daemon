# include "lib.hpp"

void exit_on_error(const std::string err) {
    std::cout << err << std::endl;
    exit(EXIT_FAILURE);
}

bool    check_rights(void) {
    return (!getuid() && !geteuid());
}

void    redir_to_devnull(void) {
    int devnull;
    if ((devnull = open("/dev/null", O_WRONLY)) == -1)
        throw customError("Error: open failed");

    if (dup2(devnull, STDIN_FILENO) == -1 || \
            dup2(devnull, STDOUT_FILENO) == -1 || \
                dup2(devnull, STDERR_FILENO) == -1) {
        close(devnull);
        throw customError("Error: dup2 failed");
    }
    close(devnull);
}

// TODO: return PID
pid_t    daemonize(bool nochdir, bool noclose) {
    pid_t   pid;
    pid = fork();
    if (pid < 0)
        throw customError("Error: fork failed");
    else if (pid > 0)
        exit(EXIT_SUCCESS);
    if (setsid() < 0)
        throw customError("Error: setsid failed");
    pid = fork();
    if (pid < 0)
        throw customError("Error: fork failed");
    if (pid > 0)
        exit(EXIT_SUCCESS);
    if (!nochdir)
        chdir("/");
    for (int x = sysconf(_SC_OPEN_MAX); x >= 3; x--) {
        close(x);
    }
    if (!noclose)
        redir_to_devnull();
    return pid;
}