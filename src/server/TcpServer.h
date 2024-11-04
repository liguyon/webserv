#pragma once

#include <map>
#include <string>

#include "../config/Config.h"

class TcpServer {
  public:
    explicit TcpServer(unsigned short port);
    explicit TcpServer(const Config& conf);
    ~TcpServer();
    void run();


  private:
    std::map<std::string, int> listeners_;

    int createSocket(const std::string& host, unsigned short port);
    TcpServer(); // = delete
    TcpServer(const TcpServer& other); // = delete
    TcpServer& operator=(const TcpServer& other); // = delete
};
