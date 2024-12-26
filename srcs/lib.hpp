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
#include <sys/file.h>
#include <string_view>
#include <cstring>

# define LOGFILE	"/var/log/matt_daemon/matt_daemon.log"
# define LOGDIR		"/var/log/matt_daemon"
# define LOCKFILE	"/var/lock/matt_daemon.lock"
# define LOG		"LOG"
# define INFO		"INFO"
# define ERROR		"ERROR"

void	exit_on_error(const std::string err);
bool	check_rights(void);
void	redir_to_devnull(void);
void	daemonize(bool nochdir, bool noclose);

class   CustomError : public std::exception {

	public:
		CustomError(const char* msg) : _msg(msg) {};
		virtual const char* what() const throw() {
			return this->_msg;
		}
	
	private:
		const char*	_msg;
	
};

class	LockFile {

	public:
		LockFile();
		~LockFile(void);
		LockFile(LockFile const & other) = default;
		LockFile& operator=(LockFile const & rhs) = default;

	private:
			int	_fd;

};

class	TintinReporter {

	public:
		TintinReporter(void);
		~TintinReporter(void) = default;
		TintinReporter(TintinReporter const & other) = default;
		TintinReporter &operator=(TintinReporter const & rhs) = default;

		void	save_logs(std::string logtype, std::string msg);

};

class	Client {
	public:
		Client(void);
		~Client(void) = default;
		Client(int fd, int *num_threads);
		Client(Client const & other) = default;
		Client &operator=(Client const & rhs) = default;

		int		get_client_fd(void) const;

		void	use_reporter(std::string msg, std::string logtype, std::mutex &mtx);
		void	increment_num_threads(std::mutex &mtx);
		void	decrement_num_threads(std::mutex &mtx);

	private:
		int					*_num_threads;
		int					_client_fd;
		TintinReporter		_reporter;
};

class	UnixSocket	{
	public:
		UnixSocket(void);
		~UnixSocket(void);
		UnixSocket(UnixSocket const & other) = default;
		UnixSocket	&operator=(UnixSocket const & rhs) = default;
		
		void	run(void);

	private:
		int					_sockfd;
		int					_num_threads;
		TintinReporter		_reporter;
		struct sockaddr_in	_addr;
};