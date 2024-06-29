# include "lib.hpp"

namespace fs = std::filesystem;

Tintin_reporter::Tintin_reporter(void) {
    if (!fs::exists(LOGDIR))
        if (!fs::create_directories(LOGDIR))
            throw std::runtime_error("Error: create_directory failed");
    if (!fs::exists(LOGFILE)) {
        std::ofstream ofs(LOGFILE);
        if (!ofs)
            throw std::runtime_error("Error: init ofstream failed");
        ofs.close();
    }
}

Tintin_reporter::~Tintin_reporter(void) {
}

void    Tintin_reporter::save_logs(std::string logtype, std::string msg) {
    time_t          rawtime;
    struct tm       *timeinfo;
    char            buffer[80];
    std::fstream    logstream;

    logstream.open(LOGFILE, std::fstream::out | std::fstream::app);
    if (!logstream.is_open())
        throw std::runtime_error("Error: open fstream failed");
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80, "%d/%m/%Y - %H:%M:%S",timeinfo);
    logstream << "["<< buffer << "] [ " << logtype << " ] - Matt_daemon: " << msg << std::endl;
    if (logstream.is_open())
        logstream.close();
}