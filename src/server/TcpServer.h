#pragma once

#include <map>
#include <string>

#include "../config/Config.h"

class TcpServer {
  public:
    static const int maxQueuedConnections = 10;
    static TcpServer* instance;

    explicit TcpServer(const Config& conf);
    ~TcpServer();
    int run();
    void shutdown();

  private:
    std::map<std::string, int> listeners_;
    int epfd_;
    bool terminate_;

    static void handleSignal(int signal);
    static int createSocket(const std::string& host, unsigned short port);
    int processEvents();
    bool isListener(int sockfd) const;
    TcpServer(); // = delete
    TcpServer(const TcpServer& other); // = delete
    TcpServer& operator=(const TcpServer& other); // = delete
};
