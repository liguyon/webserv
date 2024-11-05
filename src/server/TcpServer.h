#pragma once

#include <map>
#include <string>

#include "../config/Config.h"

class TcpServer {
  public:
    explicit TcpServer(const Config& conf);
    ~TcpServer();
    int run();
    static const int maxQueuedConnections = 10;

  private:
    std::map<std::string, int> listeners_;
    int epfd_;
    bool terminate_;

    static int createSocket(const std::string& host, unsigned short port);
    int processEvents();
    bool isListener(int sockfd) const;
    TcpServer(); // = delete
    TcpServer(const TcpServer& other); // = delete
    TcpServer& operator=(const TcpServer& other); // = delete
};
