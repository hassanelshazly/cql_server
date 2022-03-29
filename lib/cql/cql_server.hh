#pragma once

#include <arpa/inet.h>

#include "common.hh"
#include "util/assert.hh"
#include "cql_client_handler.hh"

namespace cql {

#define BACKLOG 512

class  CQLServer {

public:
  CQLServer(uint16_t port)
      : port(port), sock_listen_fd(0), logger(Logger::make()) {
    sock_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_listen_fd < 0) {
      fail("Error creating socket.");
    }

    struct sockaddr_in sock_addr;
    memset((char *)&sock_addr, 0, sizeof(sock_addr));

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_listen_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) <
        0) {
      fail("Error binding socket.");
    }

    if (listen(sock_listen_fd, BACKLOG) < 0) {
      fail("Error listening.");
    }
  }

  CQLServer(const CQLServer &) = delete;
  CQLServer &operator=(const CQLServer &) = delete;

  std::thread start() {
    return std::thread([this]() { run(); });
  }

  ~CQLServer() {
    if (sock_listen_fd > 0) {
      close(sock_listen_fd);
    }
  }

private:
  void run() {
    logger.info("server listening for connections on port: %d", port);
    struct sockaddr_in sock_addr;
    socklen_t sock_addr_len = sizeof(sock_addr);

    while (1) {
      int32_t sock_conn_fd =
          accept(sock_listen_fd, (struct sockaddr *)&sock_addr, &sock_addr_len);

      if (sock_conn_fd == -1) {
        logger.err("Error accepting new connection..");
        continue;
      }

      std::thread([this, sock_addr, sock_conn_fd]() {
        logger.info("new connection from: %s:%d", inet_ntoa(sock_addr.sin_addr),
                    ntohs(sock_addr.sin_port));
        CQLClientHandler(sock_conn_fd, sock_addr).handle_connection();
      }).detach();
    }
  }

private:
  uint16_t port;
  int32_t sock_listen_fd;
  Logger logger;
};

} // namespace cql
