# include "lib.hpp"

client::client(void) : _client_fd(0) {}

client::client(int fd, int *num_threads, std::mutex *mutex) : _num_threads(num_threads), _client_fd(fd) , _mtx(mutex){}

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
	// this->_mtx = rhs._mtx;
	return *this;
}
    

void    client::use_reporter(std::string msg) {
	std::lock_guard<std::mutex> guard(*this->_mtx);
    this->_reporter.save_logs(LOG, "User input: " + msg);
}

void    client::increment_num_threads(void) {
    std::lock_guard<std::mutex> guard(*this->_mtx);
    (*this->_num_threads) += 1;
    if (*this->_num_threads > 3)
        throw std::runtime_error("Error: num of thread cant superior than three.");
}
void    client::decrement_num_threads(void) {
    std::lock_guard<std::mutex> guard(*this->_mtx);
    (*this->_num_threads) -= 1;
    if (*this->_num_threads < 0)
        throw std::runtime_error("Error: num of thread cant be negtive.");
}