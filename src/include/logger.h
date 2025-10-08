#pragma once

#include "lockQueue.h"
#include <string>
#include <iostream>

enum LogLevel
{
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

class Logger
{
public:
    static Logger &getInstance();

    void setLogLevel(LogLevel level);
    void log(const std::string &msg);

private:
    int m_logLevel;                  // 日志级别
    LockQueue<std::string> m_lckQue; // 日志缓冲区

    Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
};

// 宏定义日志输出 (可变参数宏)  ("xxx %d %s", 20, "xxxx")
#define LOG_INFO(log_format, ...)                       \
    do                                                  \
    {                                                   \
        Logger &log = Logger::getInstance();            \
        log.setLogLevel(INFO);                          \
        char buf[1024] = {0};                           \
        snprintf(buf, 1024, log_format, ##__VA_ARGS__); \
        log.log(buf);                                   \
    } while (0);

#define LOG_ERROR(log_format, ...)                      \
    do                                                  \
    {                                                   \
        Logger &log = Logger::getInstance();            \
        log.setLogLevel(ERROR);                         \
        char buf[1024] = {0};                           \
        snprintf(buf, 1024, log_format, ##__VA_ARGS__); \
        log.log(buf);                                   \
    } while (0);

#define LOG_FATAL(log_format, ...)                      \
    do                                                  \
    {                                                   \
        Logger &log = Logger::getInstance();            \
        log.setLogLevel(FATAL);                         \
        char buf[1024] = {0};                           \
        snprintf(buf, 1024, log_format, ##__VA_ARGS__); \
        log.log(buf);                                   \
    } while (0);

#define LOG_DEBUG(log_format, ...)                      \
    do                                                  \
    {                                                   \
        Logger &log = Logger::getInstance();            \
        log.setLogLevel(DEBUG);                         \
        char buf[1024] = {0};                           \
        snprintf(buf, 1024, log_format, ##__VA_ARGS__); \
        log.log(buf);                                   \
    } while (0);
