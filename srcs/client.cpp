# include "lib.hpp"

Client::Client(void) : _client_fd(0) {}

Client::Client(int fd, int *num_threads) {
    this->_client_fd = fd;
    this->_num_threads = num_threads;
}

void    Client::use_reporter(std::string msg, std::string logtype, std::mutex &mtx) {
	std::lock_guard<std::mutex> guard(mtx);
    if (logtype == LOG)
        this->_reporter.save_logs(logtype, "User input: " + msg);
    else
        this->_reporter.save_logs(logtype, msg);
}

void    Client::increment_num_threads(std::mutex &mtx) {
    std::lock_guard<std::mutex> guard(mtx);
    (*this->_num_threads) += 1;
    if (*(this->_num_threads) > 3)
        throw CustomError("Error: num of thread cant superior than three.");
}

void    Client::decrement_num_threads(std::mutex &mtx) {
    std::lock_guard<std::mutex> guard(mtx);
    (*this->_num_threads) -= 1;
    if (*this->_num_threads < 0)
        throw CustomError("Error: num of thread cant be negtive.");
}

int     Client::get_client_fd(void) const {
    return this->_client_fd;
}