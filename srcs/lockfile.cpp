# include "lib.hpp"

namespace fs = std::filesystem;

lockfile::lockfile(void) {
    this->_fd = open(LOCKFILE, O_RDWR | O_CREAT);
    if (this->_fd == -1)
        throw customError("Error: open failed");
    if (flock(this->_fd, LOCK_EX | LOCK_NB) == -1) {
        close(this->_fd);
        if (errno == EWOULDBLOCK)
            throw customError("Error file locked.");
        throw customError("Error: flock failed");
    }    
}

lockfile::~lockfile(void) {
    if (fcntl(this->_fd, F_GETFD) != -1) {
        flock(this->_fd, LOCK_UN);
        close(this->_fd);
    }
    if (fs::exists(LOCKFILE)) 
        unlink(LOCKFILE);
}