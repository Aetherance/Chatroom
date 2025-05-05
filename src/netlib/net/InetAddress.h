#ifndef INADD_H
#define INADD_H

#include<string>
#include<arpa/inet.h>
#include<memory.h>

namespace ilib{
namespace net{
class InetAddress
{
public:
    explicit InetAddress(unsigned int port = 0);
    InetAddress(const std::string &ip,unsigned int port);
    InetAddress(const sockaddr_in & addr);

    std::string toIp() const;
    // unsigned int toPort() const;
    std::string toIpPort() const;

    const sockaddr_in& getSockAddr() const {
        return addr_;
    }

    socklen_t getSockLen() const { return sizeof(addr_); };

    bool operator==(const InetAddress & addr);
private:
    sockaddr_in addr_;
};

inline InetAddress::InetAddress(unsigned int port) {
    memset(&addr_,0,getSockLen());
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = INADDR_ANY;
}

inline InetAddress::InetAddress(const std::string &ip,unsigned int port) {
    memset(&addr_,0,getSockLen());
    addr_.sin_family = AF_INET;
    inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr);
    addr_.sin_port = htons(port);
}

inline InetAddress::InetAddress(const sockaddr_in & addr) {
    memcpy(&addr_,&addr,sizeof(addr_));
}

inline std::string InetAddress::toIpPort() const {
    std::string ip,port;
    ip.resize(1024);
    inet_ntop(AF_INET,&addr_.sin_addr,ip.data(),1024);
    port.reserve(64);
    port = std::to_string(ntohs(addr_.sin_port));
    return ip + ":" + port;
}

inline std::string InetAddress::toIp() const {
    std::string ip;
    ip.resize(1024);
    inet_ntop(AF_INET,&addr_.sin_addr,ip.data(),1024);
    return ip;
}

inline bool InetAddress::operator==(const InetAddress & addr) {
    return this->addr_.sin_addr.s_addr == addr.addr_.sin_addr.s_addr && this->addr_.sin_port == addr.addr_.sin_port;
}

}
}

#endif
