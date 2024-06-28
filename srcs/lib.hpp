# pragma once
# include <iostream>
# include <string>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <syslog.h>
# include <fcntl.h>
# include <fstream>
# include <filesystem>
# include <chrono>
# include <ctime>

# define LOGFILE    "/var/log/matt_daemon/log"
# define LOGDIR     "/var/log/matt_daemon"
# define LOCKFILE   "/var/matt_daemon.lock"
# define LOG        "LOG"
# define INFO       "INFO"
# define ERROR      "ERROR"



void    exit_on_error(const char *err);
bool    check_rights();
void    redir_to_devnull();
void    daemonize(bool nochdir, bool noclose);

class lockfile {
        public:
            lockfile(void);
            ~lockfile(void);
            lockfile(lockfile const & other) = default;
            lockfile &operator=(lockfile const & rhs) = default;

        private:
        
};

class unix_socket {
    public:
            unix_socket(void);
            ~unix_socket(void);
            unix_socket(unix_socket const & other) = default;
            unix_socket &operator=(unix_socket const & rhs) = default;

    private:
};

class Tintin_reporter {
    public:
            Tintin_reporter(void);
            ~Tintin_reporter(void);
            Tintin_reporter(Tintin_reporter const & other) = default;
            Tintin_reporter &operator=(Tintin_reporter const & rhs) = default;

            void    save_logs(const char *logtype, const char *msg);

    private:
};
