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
    Tintin_reporter reporter;
    reporter.save_logs(INFO, "Signal handler.");
    intentional_close = true;
    for (const auto & c : fds["Client"]) {
        write_to_client(c, "quit\n");
        if (shutdown(c, SHUT_RDWR) == -1)
            throw customError("Error: Shutdown failed.");
        close(c);
    }
    auto sockfd = fds["Server"].begin();
    close(*sockfd);
}

unix_socket::unix_socket(void) : _num_threads(0) {
    this->_reporter.save_logs(INFO, "Started.");
    if (fs::exists(SOCKFILE))
        unlink(SOCKFILE);
    this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_sockfd == -1)
        throw customError("Error: socket failed");
    this->_reporter.save_logs(INFO, "Creating server.");
    int reuse = 1;
    if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        throw customError("Error: setsockopt failed");
    this->_addr.sin_family = AF_INET;
    this->_addr.sin_addr.s_addr = INADDR_ANY;
    this->_addr.sin_port = htons(4242);
    if (bind(this->_sockfd, reinterpret_cast<struct sockaddr*>(&(this->_addr)), sizeof(this->_addr)) == -1)
        throw customError("Error: bind failed");
    if (listen(this->_sockfd, 3) == -1)
        throw customError("Error: listen failed");
    int signals[] = {SIGINT, SIGTERM, SIGHUP, SIGUSR1, SIGUSR2, SIGALRM, SIGPIPE, SIGCHLD, SIGCONT};
    for (int sig : signals) {
        signal(sig, handle_signals);
    }
    this->_reporter.save_logs(INFO, "Server created.");
}

unix_socket::~unix_socket(void) {
    close (this->_sockfd);
    this->_reporter.save_logs(INFO, "Quitting.");
}

unix_socket::unix_socket(unix_socket const & other) {
    *this = other;
}

unix_socket	&unix_socket::operator=(unix_socket const & rhs) {
    this->_addr = rhs._addr;
    this->_sockfd = rhs._sockfd;
    this->_num_threads = rhs._num_threads;
    return *this;
}

// while (true) {
//     n = read(fd, buf, sizeof(buf));
//     if (n == -1)
//         throw customError("Error: read failed"); 
//     buf[n] = 0;
//     memset(buf, 0, 4096);
// }
// TODO: maybe implement a dynamic method to read
void handle_client(client c, std::mutex &mtx, pid_t p) {
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
            throw customError(msg.c_str());
        } else if (n == 0)
            break ;
        buf[n] = '\0';
        msg += std::string_view(buf, n);
        if (msg == "Quit\n") {
            kill(p, SIGINT);
            break ;
        }
        std::cout << msg << std::endl;
        c.use_reporter(msg, LOG, mtx);
        msg.clear();
    }
    c.decrement_num_threads(mtx);
    c.use_reporter("Client Disconnected", INFO, mtx);
    if (fds["Client"].erase(c.get_client_fd()) != 1)
        throw customError("Error: set erase incorrect number of elements");
}

void unix_socket::run(void) {
    if ((fds["Server"].insert(this->_sockfd)).second == false)
        throw customError("Error: failed to insert sockfd in set");
    std::cout << "Welcome to Matt-Daemon : PID " << getpid() << "!" << std::endl;
    intentional_close = false;
    this->_reporter.save_logs(INFO, "Entering Daemon mode.");
    this->_reporter.save_logs(INFO, "started. PID: " + std::to_string(static_cast<int>(getpid())) + ".");
    while (true) {
        int client_fd = accept(this->_sockfd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EBADF && intentional_close == true)
                break ;
            else 
                throw customError("Error: accept failed");
        }
        else if (this->_num_threads >= 3) {
            write_to_client(client_fd, "Already 3 clients connected, please try later.");
            this->_reporter.save_logs(ERROR, "Error: Already 3 clients.");
            close(client_fd);
        } else {
            // std::unique_ptr<client> new_client(new client(client_fd, &this->_num_threads));
            // std::thread new_one(handle_client, std::move(new_client), std::ref(this->_mtx), std::ref(this->_pid));
            
            client new_client(client_fd, &this->_num_threads);
            std::thread new_one(handle_client, new_client, std::ref(this->_mtx), std::ref(this->_pid));
            
            new_one.detach();
            std::cout << "New Client id : " << client_fd << std::endl;
            if ((fds["Client"].insert(client_fd).second) == false)
                throw customError("Error: failed to insert client in set: client already connected");
            client_fd = 0;
        }
    }
}