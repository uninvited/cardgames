#include "logging.h"

#include <fstream>

namespace miplot::log {

std::ostream& operator<< (std::ostream& os, Level level)
{
    switch(level) {
        case Level::Fatal: return os << "Fatal";
        case Level::Error: return os << "Error";
        case Level::Warn: return os << "Warn";
        case Level::Info: return os << "Info";
        case Level::Debug: return os << "Debug";
    }
    return os << "Info";
}

Message::Message(Level level)
    : level_(level)
{
}

Message::~Message()
{
    auto& logger = getLogger();
    logger.log(*this);
}

Level Message::level() const
{
    return level_;
}

std::string Message::text() const
{
    return stream_.str();
}


void Logger::log(const Message& message)
{
    if (message.level() <= level()) {
        this->logImpl(message);
    }
}

LoggerFactory Logger::createLogger = []{ return toStdout(); };


class StdoutLogger : public Logger {
public:
    void logImpl(const Message& message) override
    {
        std::cout << "[" << message.level() << "] " << message.text() << "\n";
    }
};

class FileLogger : public Logger {
public:
    FileLogger(const char* fileName)
    {
        file_.open(fileName);
    }

    void logImpl(const Message& message) override
    {
        file_ << "[" << message.level() << "] " << message.text() << "\n";
    }
private:
    std::ofstream file_;
};


LoggerPtr toStdout()
{
    return std::make_shared<StdoutLogger>();
}

LoggerPtr toFile(const char* fileName)
{
    return std::make_shared<FileLogger>(fileName);
}

void setLogger(LoggerPtr logger)
{
    Logger::createLogger = [logger]() { return logger; };
}

Logger& getLogger()
{
    static LoggerPtr instance = Logger::createLogger();
    return *instance;
}

void setLogLevel(Level level)
{
    getLogger().setLevel(level);
}

Level getLogLevel()
{
    return getLogger().level();
}

} // namespace miplot::log
