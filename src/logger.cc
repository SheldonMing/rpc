#include "logger.h"

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{
    std::thread([&]()
                {
        while(true){
            time_t now = time(nullptr);
            tm *now_tm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%04d-%02d-%02d-log.txt", 
                                now_tm->tm_year + 1900, 
                                now_tm->tm_mon + 1, 
                                now_tm->tm_mday);
            
            FILE *pf = fopen(file_name, "a+");
            if(pf == nullptr){
                std::cout << "open log file " << file_name << " failed!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckQue.pop();

            char time_buf[128] = {0};
            sprintf(time_buf,"%02d:%02d:%02d=>[%s] ", 
                                now_tm->tm_hour,
                                now_tm->tm_min, 
                                now_tm->tm_sec,
                                m_logLevel == INFO ? "info" :
                                m_logLevel == ERROR ? "error" :
                                m_logLevel == FATAL ? "fatal" :
                                m_logLevel == DEBUG ? "debug" : "other");
            msg.insert(0, time_buf);
            msg.append("\n");
            fclose(pf);
        } });
}

void Logger::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::log(const std::string &msg)
{
    m_lckQue.push(msg);
}
