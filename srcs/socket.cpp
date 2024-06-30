# include "lib.hpp"
# include <map>
# include <unordered_set>

namespace fs = std::filesystem;

std::map<std::string, std::unordered_set<int>>    fds;
bool    intentional_close;

void    write_to_client(int client_fd, std::string msg) {
    send(client_fd, msg.c_str(), msg.size(), 0);
}

bool is_fd_open(int fd) {
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

void handle_signals(int signal) {
    (void)signal;
    TintinReporter reporter;
    reporter.save_logs(INFO, "Signal handler.");
    intentional_close = true;
    for (const auto & c : fds["Client"]) {
        write_to_client(c, "quit\n");
        if (shutdown(c, SHUT_RDWR) == -1)
            throw CustomError("Error: Shutdown failed.");
        close(c);
    }
    auto sockfd = fds["Server"].begin();
    close(*sockfd);
}

UnixSocket::UnixSocket(void) : _num_threads(0) {
    this->_reporter.save_logs(INFO, "Started.");
    this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_sockfd == -1)
        throw CustomError("Error: socket failed");
    this->_reporter.save_logs(INFO, "Creating server.");
    int reuse = 1;
    if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        throw CustomError("Error: setsockopt failed");
    this->_addr.sin_family = AF_INET;
    this->_addr.sin_addr.s_addr = INADDR_ANY;
    this->_addr.sin_port = htons(4242);
    if (bind(this->_sockfd, reinterpret_cast<struct sockaddr*>(&(this->_addr)), sizeof(this->_addr)) == -1)
        throw CustomError("Error: bind failed");
    if (listen(this->_sockfd, 3) == -1)
        throw CustomError("Error: listen failed");
    int signals[] = {SIGINT, SIGTERM, SIGHUP, SIGUSR1, SIGUSR2, SIGALRM, SIGPIPE, SIGCHLD, SIGCONT};
    for (int sig : signals) {
        signal(sig, handle_signals);
    }
    this->_reporter.save_logs(INFO, "Server created.");
}

UnixSocket::~UnixSocket(void) {
    close (this->_sockfd);
    this->_reporter.save_logs(INFO, "Quitting.");
}

// while (true) {
//     n = read(fd, buf, sizeof(buf));
//     if (n == -1)
//         throw CustomError("Error: read failed"); 
//     buf[n] = 0;
//     memset(buf, 0, 4096);
// }
// TODO: maybe implement a dynamic method to read
void handle_client(Client c, std::mutex &mtx) {
    std::cout << "handle_client " << c.get_client_fd() << std::endl;
    int         n;
    int         fd;
    char        buf[4096];
    std::string msg;

    fd = c.get_client_fd();
    c.use_reporter("New Client", INFO, mtx);
    c.increment_num_threads(mtx);
    while (true) {
        memset(buf, 0, 4096);
    std::cout << "recv " << fd << std::endl;
        n = recv(fd, buf, sizeof(buf), 0);
        if (n == -1) {
            std::cerr << "recv failed with error: " << strerror(errno) << std::endl;
            std::string err = "Error: recv failed with error: " + std::string(strerror(errno));
            throw CustomError(msg.c_str());
        } else if (n == 0)
            break ;
        buf[n] = '\0';
        msg += std::string_view(buf, n);
        if (msg == "Quit\n") {
            kill(getpid(), SIGINT);
            break ;
        }
        std::cout << msg << std::endl;
        c.use_reporter(msg, LOG, mtx);
        msg.clear();
    }
    c.decrement_num_threads(mtx);
    c.use_reporter("Client Disconnected", INFO, mtx);
    if (fds["Client"].erase(c.get_client_fd()) != 1)
        throw CustomError("Error: set erase incorrect number of elements");
}

void UnixSocket::run(void) {
    std::mutex			mtx;
    if ((fds["Server"].insert(this->_sockfd)).second == false)
        throw CustomError("Error: failed to insert sockfd in set");
    intentional_close = false;
    std::cout << "Welcome to Matt-Daemon : PID " << getpid() << "!" << std::endl;
    this->_reporter.save_logs(INFO, "Entering Daemon mode.");
    this->_reporter.save_logs(INFO, "started. PID: " + std::to_string(static_cast<int>(getpid())) + ".");
    while (true) {
        int client_fd = accept(this->_sockfd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EBADF && intentional_close == true)
                break ;
            else 
                throw CustomError("Error: accept failed");
        }
        else if (this->_num_threads >= 3) {
            write_to_client(client_fd, "Already 3 clients connected, please try later.");
            this->_reporter.save_logs(ERROR, "Error: Already 3 clients.");
            close(client_fd);
        } else {
            Client new_client(client_fd, &this->_num_threads);
            std::thread new_one(handle_client, new_client, std::ref(mtx));
            new_one.detach();
            std::cout << "New Client id : " << client_fd << std::endl;
            if ((fds["Client"].insert(client_fd).second) == false)
                throw CustomError("Error: failed to insert Client in set: Client already connected");
            client_fd = 0;
        }
    }
}