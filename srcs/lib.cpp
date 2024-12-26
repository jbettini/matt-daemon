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
        throw CustomError("Error: open failed");

    (void)dup2(devnull, STDIN_FILENO);
    (void)dup2(devnull, STDOUT_FILENO);
    (void)dup2(devnull, STDERR_FILENO);
    if (devnull > 2)
        (void)close (devnull);
}

// TODO: return PID
void   daemonize(bool nochdir, bool noclose) {
    pid_t   pid;
    pid = fork();
    if (pid < 0)
        throw CustomError("Error: fork failed");
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0)
        throw CustomError("Error: setsid failed");

    pid = fork();
    if (pid < 0)
        throw CustomError("Error: fork failed");
    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (!nochdir)
        chdir("/");
    // for (int x = sysconf(_SC_OPEN_MAX); x >= 3; x--) {
    //     close(x);
    // }
    if (!noclose)
        redir_to_devnull();
    std::cout << "Successfully Daemonize with " << getpid() << std::endl;
}