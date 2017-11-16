#ifndef LSP_LOGGER_H
#define LSP_LOGGER_H
#include <string>
#include <iostream>

namespace lsp {
class Logger {
    public:
    virtual void logError(const std::string &Message) = 0;
    virtual void logInfo(const std::string &Message) = 0;
    virtual void logWarning(const std::string &Message) = 0;
};

class StdIoLogger : public Logger {
    public:
    virtual void logError(const std::string &Message) {
        std::cerr << "Error: " << Message << std::endl;
    }
    virtual void logInfo(const std::string &Message) {
        std::cout << "Info: " << Message << std::endl;
    }
    virtual void logWarning(const std::string &Message) {
        std::cerr << "Warning: " << Message << std::endl;
    }
};

};
#endif