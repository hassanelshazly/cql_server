#pragma once

#include "common.hh"
#include "cql_message.hh"
#include "cql_parser.hh"
#include "util/assert.hh"

namespace cql {

class  CQLMessageParser : public CQLParser {
public:
  CQLMessageParser(const std::string &data) : CQLParser(data) {}

  CQLMessage parse() {
    if (remaining_size() < CQLMessage::HEADER_LENGTH) {
      fail("CQLMessageParser::parse: data size is less than header length");
      return CQLMessage();
    }

    uint8_t type_version = parse_byte();
    uint8_t type = type_version >> 0x7;
    uint8_t version = type_version & 0x7F;
    uint8_t flags = parse_byte();
    uint16_t stream_id = parse_short();
    uint8_t opcode = parse_byte();
    std::string body = parse_bytes();

    return CQLMessage(static_cast<CQLMessage::Type>(type), version, flags,
                      stream_id, static_cast<CQLMessage::Opcode>(opcode), body);
  }
};

} // namespace cql
