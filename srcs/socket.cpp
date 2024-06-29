# include "lib.hpp"

namespace fs = std::filesystem;

void    write_to_client(int client_fd, std::string msg) {
    send(client_fd, msg.c_str(), msg.size(), 0);
}

unix_socket::unix_socket(void) : _num_threads(0) {
    this->_reporter.save_logs(INFO, "Started.");
    try {
        this->_lock = lockfile();
    } catch (const std::exception &e) {
        this->_reporter.save_logs(ERROR, "Error file locked.");
        throw e;
    }
    if (fs::exists(SOCKFILE))
        unlink(SOCKFILE);
    this->_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (this->_sockfd == -1)
        throw std::runtime_error("Error: socket failed");
    this->_reporter.save_logs(INFO, "Creating server.");
    memset(&this->_addr, 0, sizeof(struct sockaddr_un));
    this->_addr.sun_family = AF_UNIX;
    strncpy(this->_addr.sun_path, SOCKFILE, sizeof(this->_addr.sun_path) - 1);

    if (bind(this->_sockfd, (struct sockaddr*)&this->_addr, sizeof(struct sockaddr_un)) == -1)
        throw std::runtime_error("Error: bind failed");
    if (listen(this->_sockfd, 3) == -1)
        throw std::runtime_error("Error: listen failed");
    this->_reporter.save_logs(INFO, "Server created.");
}

unix_socket::~unix_socket(void) {
    close (this->_sockfd);
    if (fs::exists(SOCKFILE))
        unlink(SOCKFILE);
    this->_reporter.save_logs(INFO, "Quitting.");
}

// TODO: increment and decrement thread count using mutex
void handle_client(client c) {
}

void unix_socket::run(void) {
    while (true) {
        int client_fd = accept(this->_sockfd, NULL, NULL);
        if (client_fd == -1)
            throw std::runtime_error("Error: accept failed");
        else if (this->_num_threads >= 3) {
            write_to_client(client_fd, "Already 3 clients connected, please try later.");
            this->_reporter.save_logs(ERROR, "Error: Already 3 clients.");
            close(client_fd);
        } else {
            client new_client(client_fd, &this->_num_threads, &this->_mtx);
            std::thread new_one(handle_client, new_client);
			new_one.detach();
        }
    }
}