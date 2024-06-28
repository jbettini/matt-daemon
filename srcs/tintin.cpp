# include "lib.hpp"

namespace fs = std::filesystem;

Tintin_reporter::Tintin_reporter(void) {
    if (!fs::exists(LOGDIR))
        if (!fs::create_directories(LOGDIR))
            exit_on_error("Error: Cannot create logdir");
    if (!fs::exists(LOGFILE)) {
        std::ofstream ofs(LOGFILE);
        if (!ofs)
            exit_on_error("Error: Cannot create logfile");
        ofs.close();
    }
    this->save_logs(INFO, "Started.");
}

Tintin_reporter::~Tintin_reporter(void) {
    this->save_logs(INFO, "Quitting.");
}

void    Tintin_reporter::save_logs(const char *logtype, const char *msg) {
    time_t          rawtime;
    struct tm       *timeinfo;
    char            buffer[80];
    std::fstream    logstream;

    logstream.open(LOGFILE, std::fstream::out | std::fstream::app);
    if (!logstream.is_open())
        exit_on_error("Error: Cannot open logfile");

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80, "%d/%m/%Y - %H:%M:%S",timeinfo);
    logstream << "["<< buffer << "] [ " << logtype << " ] - Matt_daemon: " << msg << std::endl;
    if (logstream.is_open())
        logstream.close();
}