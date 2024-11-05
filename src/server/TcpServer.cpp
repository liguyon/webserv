#include "TcpServer.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <exception>

#include "../utils/Log.h"
#include "../utils/String.h"

static int epollCtlAdd(int epfd, int fd, unsigned int event);

TcpServer::TcpServer(const Config& conf) : terminate_(false) {
  if ((epfd_ = epoll_create1(EPOLL_CLOEXEC) == -1)) {
    logger.error("Server initialization failed. Failed to create epoll instance.");
    throw std::exception();
  }
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
  close(epfd_);
}

int epollCtlAdd(int epfd, int fd, unsigned int events) {
  epoll_event ev = {};
  ev.events = events;
  ev.data.fd = fd;
  return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1;
}

int TcpServer::run() {
  for (
    std::map<std::string, int>::const_iterator it = listeners_.begin();
    it != listeners_.end();
    ++it) {
    if (listen(it->second, maxQueuedConnections) == -1) {
      logger.error(
        "Server initialization failed. Failed to listen on socket at "
        + it->first + ": " + std::string(strerror(errno)) + '.'); // errno: no read write
      return EXIT_FAILURE;
    }
    if (epollCtlAdd(epfd_, it->second, EPOLLIN | EPOLLOUT | EPOLLET) == -1) {
      logger.error("Server initialization aborted: epoll_ctl failed.");
      return EXIT_FAILURE;
    }
    logger.info("Server listening at " + it->first + ".");
  }
  // int status = 0;
  // while (!terminate_) {
  //   status = processEvents();
  //   if (status != EXIT_SUCCESS)
  //     return status;
  // }
  // return status;
  return 0;
}

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

// int TcpServer::processEvents() {
//   // TODO: implement process event
// }

bool TcpServer::isListener(int sockfd) const {
  for (
    std::map<std::string, int>::const_iterator it = listeners_.begin();
    it != listeners_.end();
    ++it) {
    if (it->second == sockfd)
      return true;
  }
  return false;
}
