#include "TcpServer.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <exception>

#include "../utils/Log.h"
#include "../utils/String.h"

TcpServer::TcpServer(const Config& conf) {
  for (
    std::vector<Config::ServerBlock>::const_iterator it = conf.getServerConfig().begin();
    it != conf.getServerConfig().end();
    ++it
  ) {
    int sock = createSocket(it->host, it->port);
    if (sock == -1)
      throw std::exception();
    std::string addr = it->host + ":" + String::fromInt(it->port);
    listeners_[addr] = sock;
  }
}

TcpServer::~TcpServer() {
  for (
    std::map<std::string, int>::const_iterator it = listeners_.begin();
    it != listeners_.end();
    ++it) {
    close(it->second);
    logger.info("Server shut down at " + it->first + '.');
  }
}

// void TcpServer::run() {
//   if (listen(listener_, 10) == -1) {
//     logger.error(
//       "Server initialization failed. Failed to listen on socket at "
//       + addrStr_ + ':' + portStr_ + ": " + std::string(strerror(errno)) + '.');
//     close(listener_);
//     freeaddrinfo(servInfo_);
//     throw std::exception();
//   }
// }

int TcpServer::createSocket(const std::string& host, unsigned short port) {
  std::string portStr = String::fromInt(port);
  addrinfo* servInfo;
  int status;

  // addrinfo
  addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((status = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &servInfo)) != 0) {
    logger.error(
      "Server initialization failed. Failed to resolve "
      + host + ":" + portStr + ": "
      + std::string(gai_strerror(status)) + '.');
    return -1;
  }

  // socket
  // TODO: loop through servInfo to try to create a socket
  int listener;
  if ((listener = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol)) == -1) {
    logger.error(
      "Server initialization failed. Failed to create socket for "
      + host + ":" + portStr + ": "
      + std::string(strerror(errno)) + '.'); // errno allowed, this is not a read/write
    freeaddrinfo(servInfo);
    return -1;
  }
  fcntl(listener, F_SETFL, O_NONBLOCK);
  if (bind(listener, servInfo->ai_addr, servInfo->ai_addrlen) == -1) {
    logger.error(
      "Server initialization failed. Failed to bind socket to "
      + host + ':' + portStr + ": " + std::string(strerror(errno)) + '.'); // errno allowed, this is not a read/write
    close(listener);
    freeaddrinfo(servInfo);
    return -1;
  }
  freeaddrinfo(servInfo);
  return listener;
}
