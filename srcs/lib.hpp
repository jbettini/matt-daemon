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
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string_view>

// struct sockaddr_in {
//     short            sin_family;   // e.g. AF_INET
//     unsigned short   sin_port;     // e.g. htons(3490)
//     struct in_addr   sin_addr;     // see struct in_addr, below
//     char             sin_zero[8];  // zero this if you want to
// };



# define LOGFILE	"/var/log/matt_daemon/log"
# define LOGDIR		"/var/log/matt_daemon"
# define LOCKFILE	"/var/matt_daemon.lock"
# define SOCKFILE	"/var/matt_daemon.sock"
# define LOG		"LOG"
# define INFO		"INFO"
# define ERROR		"ERROR"

void	exit_on_error(const std::string err);
bool	check_rights(void);
void	redir_to_devnull(void);
pid_t	daemonize(bool nochdir, bool noclose);

class   customError : public std::exception {
	public:
		customError(const char* msg) : _msg(msg) {};
		virtual const char* what() const throw() {
			return this->_msg;
		}
	private:
		const char*	_msg;
};

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
		client(int fd, int *num_threads);
		client(client const & other);
		client &operator=(client const & rhs);

		int		get_client_fd(void) const;

		void	use_reporter(std::string msg, std::string logtype, std::mutex &mtx);
		void	increment_num_threads(std::mutex &mtx);
		void	decrement_num_threads(std::mutex &mtx);

	private:
		int					*_num_threads;
		int					_client_fd;
		Tintin_reporter		_reporter;
};

class	unix_socket	{
	public:
		unix_socket(void);
		~unix_socket(void);
		unix_socket(unix_socket const & other);
		unix_socket	&operator=(unix_socket const & rhs);
		
		void	set_pid(void);
		void	run(void);

	private:
		int					_sockfd;
		int					_num_threads;
		pid_t				_pid;
		std::mutex			_mtx;
		Tintin_reporter		_reporter;
		struct sockaddr_in	_addr;
};