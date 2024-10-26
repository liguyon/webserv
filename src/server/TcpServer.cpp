#include "TcpServer.h"

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <exception>
#include <sstream>

#include "../utils/Log.h"

std::string usToString(unsigned short s) {
  std::ostringstream ss;
  ss << s;
  return ss.str();
}

TcpServer::TcpServer(unsigned short port)
  : port_(port), portStr_(usToString(port)) {
  // populate addrinfo
  int status;
  addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((status = getaddrinfo(NULL, portStr_.c_str(), &hints, &servInfo_)) != 0) {
    logger.error(
      "TcpServer initialization failed. Failed to resolve host address using getaddrinfo: "
      + std::string(gai_strerror(status)) + '.');
    throw std::exception();
  }

  // get address as string
  sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(servInfo_->ai_addr);
  void* addr = &(ipv4->sin_addr);
  char ipStr[INET_ADDRSTRLEN];
  inet_ntop(servInfo_->ai_family, addr, ipStr, sizeof ipStr);
  addrStr_ = ipStr;

  // socket
  if ((listener_ = socket(servInfo_->ai_family, servInfo_->ai_socktype, servInfo_->ai_protocol)) == -1) {
    logger.error("TcpServer initialization failed. Failed to create socket: " + std::string(strerror(errno)) + '.');
    freeaddrinfo(servInfo_);
    throw std::exception();
  }
  if (bind(listener_, servInfo_->ai_addr, servInfo_->ai_addrlen) == -1) {
    logger.error(
      "TcpServer initialization failed. Failed to bind socket to "
      + addrStr_ + ':' + portStr_ + ": " + std::string(strerror(errno)) + '.');
    close(listener_);
    freeaddrinfo(servInfo_);
    throw std::exception();
  }
  if (listen(listener_, 10) == -1) {
    logger.error(
      "TcpServer initialization failed. Failed to listen on socket at "
      + addrStr_ + ':' + portStr_ + ": " + std::string(strerror(errno)) + '.');
    close(listener_);
    freeaddrinfo(servInfo_);
    throw std::exception();
  }

  logger.info("TcpServer listening at " + addrStr_ + ':' + portStr_ + '.');
}

TcpServer::~TcpServer() {
  close(listener_);
  freeaddrinfo(servInfo_);
  logger.info("TcpServer shut down at " + addrStr_ + ':' + portStr_ + '.');
}
