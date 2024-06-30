# include "lib.hpp"

client::client(void) : _client_fd(0) {}

client::client(int fd, int *num_threads) {
    this->_client_fd = fd;
    this->_num_threads = num_threads;
}

client::~client(void) {
    close(this->_client_fd);
}

client::client(client const & other) {
	*this = other;
}

client &client::operator=(client const & rhs) {
	this->_num_threads = rhs._num_threads;
	this->_client_fd = rhs._client_fd;
	this->_reporter = rhs._reporter;
	return *this;
}
    

void    client::use_reporter(std::string msg, std::string logtype, std::mutex &mtx) {
	std::lock_guard<std::mutex> guard(mtx);
    if (logtype == LOG)
        this->_reporter.save_logs(logtype, "User input: " + msg);
    else
        this->_reporter.save_logs(logtype, msg);
}

void    client::increment_num_threads(std::mutex &mtx) {
    std::lock_guard<std::mutex> guard(mtx);
    (*this->_num_threads) += 1;
    if (*(this->_num_threads) > 3)
        throw customError("Error: num of thread cant superior than three.");
}

void    client::decrement_num_threads(std::mutex &mtx) {
    std::lock_guard<std::mutex> guard(mtx);
    (*this->_num_threads) -= 1;
    if (*this->_num_threads < 0)
        throw customError("Error: num of thread cant be negtive.");
}

int     client::get_client_fd(void) const {
    return this->_client_fd;
}