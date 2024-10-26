#pragma once

#include <netdb.h>
#include <string>

class TcpServer {
  public:
    explicit TcpServer(unsigned short port);
    ~TcpServer();


  private:
    unsigned short port_;
    std::string portStr_;
    std::string addrStr_;
    addrinfo* servInfo_;
    int listener_;

    TcpServer();
    TcpServer(const TcpServer& other);
    TcpServer& operator=(const TcpServer& other);
};
