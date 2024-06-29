# pragma once
# include <iostream>
# include <string>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <syslog.h>
# include <fcntl.h>
# include <fstream>
# include <filesystem>
# include <chrono>
# include <ctime>
# include <thread>

# define LOGFILE	"/var/log/matt_daemon/log"
# define LOGDIR		"/var/log/matt_daemon"
# define LOCKFILE	"/var/matt_daemon.lock"
# define SOCKFILE	"/var/matt_daemon.sock"
# define LOG		"LOG"
# define INFO		"INFO"
# define ERROR		"ERROR"


struct	sockaddr_un	{
	sa_family_t	sun_family;					/* AF_UNIX */
	char		sun_path[108];				/* Pathname */
};

void	exit_on_error(const std::string err);
bool	check_rights(void);
void	redir_to_devnull(void);
void	daemonize(bool nochdir, bool noclose);

class	lockfile {
	public:
		lockfile();
		~lockfile(void);
		lockfile(lockfile const & other) = default;
		lockfile	&operator=(lockfile const & rhs) = default;

	private:
			int	_fd;
};

class	Tintin_reporter {
	public:
		Tintin_reporter(void);
		~Tintin_reporter(void);
		Tintin_reporter(Tintin_reporter const & other) = default;
		Tintin_reporter &operator=(Tintin_reporter const & rhs) = default;

		void	save_logs(std::string logtype, std::string msg);

	private:
};

class	client {
	public:
		client(void);
		~client(void);
		client(int fd, int *num_threads, std::mutex *mtx);
		client(client const & other);
		client &operator=(client const & rhs);

		void	use_reporter(std::string msg);
		void	increment_num_threads(void);
		void	decrement_num_threads(void);

	private:
		int					*_num_threads;
		int					_client_fd;
		std::mutex			*_mtx;
		Tintin_reporter		_reporter;
};

class	unix_socket	{
	public:
		unix_socket(void);
		~unix_socket(void);
		unix_socket(unix_socket const & other) = default;
		unix_socket	&operator=(unix_socket const & rhs) = default;
		
		void	run(void);

	private:
		int					_sockfd;
		int					_num_threads;
		std::mutex			_mtx;
		lockfile			_lock;
		Tintin_reporter		_reporter;
		struct sockaddr_un	_addr;
};