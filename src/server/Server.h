#pragma once

#include <netdb.h>
#include <string>

class Server {
  public:
    explicit Server(unsigned short port);
    ~Server();


  private:
    unsigned short port_;
    std::string portStr_;
    std::string addrStr_;
    addrinfo* servInfo_;
    int listener_;

    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
};
