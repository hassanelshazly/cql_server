#pragma once

#include <arpa/inet.h>
#include <unistd.h>

#include "common.hh"
#include "cql_message_builder.hh"
#include "cql_message_parser.hh"
#include "cql_query_parser.hh"
#include "cql_query_processor.hh"
#include "cql_result_builder.hh"

namespace cql {

#define MAX_MESSAGE_LEN 10240

class CQLClientHandler {
public:
  CQLClientHandler(int32_t sock_conn_fd, struct sockaddr_in sock_addr)
      : sock_conn_fd(sock_conn_fd), sock_addr(sock_addr),
        logger(Logger::make()){};

  CQLClientHandler(const CQLClientHandler &) = delete;
  CQLClientHandler &operator=(const CQLClientHandler &) = delete;

  void handle_connection() {
    char buffer[MAX_MESSAGE_LEN];
    while (1) {
      int32_t bytes_received = read(sock_conn_fd, buffer, MAX_MESSAGE_LEN);

      if (bytes_received <= 0) {
        logger.info("connection from %s:%d closed",
                    inet_ntoa(sock_addr.sin_addr), ntohs(sock_addr.sin_port));
        close(sock_conn_fd);
        break;
      }

      CQLMessageParser parser(std::string(buffer, bytes_received));

      while (parser.remaining_size() >= CQLMessage::HEADER_LENGTH) {
        CQLMessage cql_message = parser.parse();

        logger.info("\nNew MESSSAGE");
        logger.info("message_length: %d", parser.size());
        logger.info("remaining_length: %d", parser.remaining_size());
        logger.info("message: %s", cql_message.to_string().c_str());

        std::string response = process_message(cql_message);
        write(sock_conn_fd, response.c_str(), response.length());
      }
    }
  }

private:
  std::string process_message(const CQLMessage &cql_message) {
    CQLMessageBuilder cql_response_builder;
    int16_t stream_id = cql_message.stream_id();

    // TODO: support rest of the message types
    switch (cql_message.opcode()) {
    case CQLMessage::Opcode::STARTUP:
      logger.info("\nSTARTUP");
      cql_response_builder = CQLMessageBuilder::from_ready(stream_id);
      break;
    case CQLMessage::Opcode::REGISTER:
      logger.info("\nREGISTER");
      cql_response_builder = CQLMessageBuilder::from_ready(stream_id);
      break;
    case CQLMessage::Opcode::OPTIONS:
      logger.info("\nOPTIONS");
      cql_response_builder = CQLMessageBuilder::from_supported(stream_id);
      break;
    case CQLMessage::Opcode::QUERY:
      logger.info("\nQUERY");
      cql_response_builder = CQLMessageBuilder::from_result(
          stream_id, CQLQueryProcessor(cql_message.body()).serialize());
      break;
    default:
      logger.info("\nUNKNOWN %d", cql_message.opcode());
      cql_response_builder = CQLMessageBuilder::from_error(
          stream_id, CQLMessage::Opcode::ERROR, "Not Implemented Operation");
      break;
    }

    CQLMessage cql_response = cql_response_builder.build();
    std::string response = cql_response.serialize();

    logger.info("response_length: %d", response.length());
    logger.info("response: %s", cql_response.to_string().c_str());

    return response;
  }

private:
  int32_t sock_conn_fd;
  struct sockaddr_in sock_addr;
  Logger logger;
};

} // namespace cql
