#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>

namespace miplot::log {

enum class Level { Fatal, Error, Warn, Info, Debug };

std::ostream& operator<< (std::ostream& os, Level level);

class Message {
public:
    explicit Message(Level level);

    ~Message();

    Level level() const;

    std::string text() const;

    template <typename T>
    Message& operator<<(const T& val)
    {
        stream_ << val;
        return *this;
    }
private:
    Level level_;
    std::ostringstream stream_;
};

class Logger;
using LoggerPtr = std::shared_ptr<Logger>;
using LoggerFactory = std::function<LoggerPtr()>;

// Simple non-threadsafe logger
class Logger {
public:
    void log(const Message&);
    Level level() const { return level_; }
    void setLevel(Level level) { level_ = level; }

    virtual void logImpl(const Message&) = 0;

    static LoggerFactory createLogger;
private:
    Level level_ = Level::Info;
};

LoggerPtr toStdout();
LoggerPtr toFile(const char* fileName);

void setLogger(LoggerPtr);
Logger& getLogger();

void setLogLevel(Level level);
Level getLogLevel();

#define LOG(level) miplot::log::Message(level)

#define FATAL() LOG(miplot::log::Level::Fatal)
#define ERROR() LOG(miplot::log::Level::Error)
#define WARN()  LOG(miplot::log::Level::Warn)
#define INFO()  LOG(miplot::log::Level::Info)
#define DEBUG() LOG(miplot::log::Level::Debug)

} // namespace miplot::log
