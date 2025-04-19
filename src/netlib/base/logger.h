#ifndef LOG_H
#define LOG_H

#include<time.h>
#include<string.h>
#include<iostream>
#include<string>
#include<arpa/inet.h>
#include<fstream>
#include"noncopyable.h"

#define CONNECT_ON "connected"
#define CONNECT_OFF "disconnected!"

#define COLOR_RESET    "\033[0m"
#define COLOR_CYAN     "\033[1m\033[36m"              // INFO_SUCCESS
#define COLOR_YELLOW   "\033[93m"                     // WARNING
#define COLOR_RED      "\033[91m"                     // ERROR
#define COLOR_BOLD_RED "\033[1;31m"                   // FATAL
#define COLOR_RED_BG   "\033[41;37m"                  // FATAL++
#define COLOR_GREEN    "\033[032m"
#define COLOR_GREEN__  "\033[1m\033[032m"             // CONNECT
#define COLOR_RED__    "\033[1m\033[31m"              // DISCONNECT

#define LOG_INFO(event) ilib::log::logger::serverlog(ilib::log::INFO,event);
#define LOG_WARN(event) ilib::log::logger::serverlog(ilib::log::WARNING,event);
#define LOG_ERROR(event) ilib::log::logger::serverlog(ilib::log::ERROR,event);
#define LOG_FATAL(event) ilib::log::logger::serverlog(ilib::log::FATAL,event);
#define LOG_INFO_SUCCESS(event) ilib::log::logger::serverlog(ilib::log::INFO_SUCCESS,event);

#define LOG_CLIENT_INFO_ADDR(event,ip_port) ilib::log::logger::clientlog(ilib::log::INFO,ip_port,event);
#define LOG_CLIENT_INFO(event,fd) ilib::log::logger::clientlog(ilib::log::INFO,fd,event);
#define LOG_CLIENT_WARN(event,fd) ilib::log::logger::clientlog(ilib::log::WARNING,fd,event);
#define LOG_CLIENT_ERROR(event,fd) ilib::log::logger::clientlog(ilib::log::ERROR,fd,event);
#define LOG_CLIENT_FATAL(event,fd) ilib::log::logger::clientlog(ilib::log::FATAL,fd,event);

namespace ilib {
namespace log {

enum STAT {
    INFO_SUCCESS,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class logger : noncopyable
{    
public:
    static void serverlog(int type,std::string event);
    static void clientlog(int type,int fd,std::string event);
    static void clientlog(int type,std::string ip_port,std::string event);
private:
    void log_server(int type,std::string event);
    void log_client(int type,std::string ip,unsigned int port,std::string event);
    void log_client(int type,int fd,std::string event);

    logger(bool isTerminal,bool isLogfile);
    logger();
    ~logger();
    
    bool isTerminal;
    bool isLogfile;
    std::ofstream logfile;
};

inline logger::logger() {
    isTerminal = true,isLogfile = false;
}

inline logger::logger(bool isTerminal,bool isLogfile) {
    this->isTerminal = isTerminal,this->isLogfile = isLogfile;
    if(isLogfile) {
        logfile = std::ofstream("server.log",std::ios::out | std::ios::app);
    }
    logfile.rdbuf()->pubsetbuf(nullptr,0);
}

inline void logger::log_client(int type,std::string ip,unsigned int port,std::string event) {
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    const char* color = "\033[0m"; 
    const char* reset_color = "\033[0m";

    if(event == CONNECT_ON) {
        color = COLOR_GREEN__;
    } else if(event == CONNECT_OFF) {
        color = COLOR_RED__;
    }

    std::string info_type;
    switch (type) {
    case INFO_SUCCESS:
        info_type = "INFO:SUCCESS";
        break;
    case INFO:
        info_type = "INFO";
        break;
   
    case WARNING:
        color = COLOR_YELLOW;
        info_type = "WARNING";
        break;
   
    case ERROR:
        color = COLOR_RED;
        info_type = "ERROR";
        break;

    case FATAL:
        color = COLOR_RED_BG;
        info_type = "FATAL";
        break;

    default:
        perror("log: type");
        break;
    }

    if(isTerminal) {
        printf("[%s] [%s] %sClient %s:%d %s%s\n", 
            time_str, info_type.c_str(), color, ip.c_str(), port, event.c_str(), COLOR_RESET);
    }

    if(isLogfile) {
        char buff[1024];
        sprintf(buff,"[%s] [%s] Client %s:%d %s\n", 
            time_str, info_type.c_str(), ip.c_str(), port, event.c_str());
        logfile<<buff;
    }
    
}

inline void logger::log_client(int type,int fd,std::string event) {
    sockaddr_in sin;
    socklen_t size = sizeof(sockaddr_in);
    getpeername(fd,(sockaddr*)&sin,&size);
    char ip[16];
    int port;
    inet_ntop(AF_INET,(sockaddr*)&sin.sin_addr,ip,sizeof(sockaddr_in));
    port = ntohs(sin.sin_port);
    log_client(type,ip,port,event);
}

inline logger::~logger() {
    if(isLogfile) logfile.close();
}

inline void logger::log_server(int type,std::string event) {
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    const char* color = "\033[0m"; 
    const char* reset_color = "\033[0m";

    std::string info_type;
    switch (type) {
    case INFO_SUCCESS:
        info_type = "INFO:SUCCESS";
        color = COLOR_CYAN;
        break;

    case INFO:
        info_type = "INFO";
        break;
   
    case WARNING:
        color = COLOR_YELLOW;
        info_type = "WARNING";
        break;
   
    case ERROR:
        color = COLOR_RED;
        info_type = "ERROR";
        break;

    case FATAL:
        color = COLOR_RED_BG;
        info_type = "FATAL";
        break;

    default:
        perror("log: type");
        break;
    }

    if(isTerminal) {
        printf("%s[%s] [%s] Server: %s%s\n", 
            color, time_str, info_type.c_str(),event.data(),reset_color);
    }

    if(isLogfile) {
        char buff[1024];
        sprintf(buff,"[%s] [%s] Server: %s%s\n", 
            time_str, info_type.c_str(),event.data(),reset_color);
        logfile<<buff;
    }

}

inline void logger::serverlog(int type,std::string event) {
    static logger logger_;
    logger_.log_server(type,event);
}

inline void logger::clientlog(int type,int fd,std::string event) {
    static logger logger_;
    logger_.log_client(type,fd,event);
}

inline void logger::clientlog(int type,std::string ip_port,std::string event) {
    std::string ip(ip_port,0,ip_port.find(':'));
    std::string port(ip_port,ip_port.find(':')+1,ip_port.size());
    logger log;
    log.log_client(type,ip,std::stoi(port),event);
}

}
}

#endif