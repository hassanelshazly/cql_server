#pragma once

#include "common.hh"
#include "cql_message.hh"
#include "cql_serializer.hh"

namespace cql {

class  CQLMessageBuilder {
public:
  CQLMessageBuilder() = default;

  CQLMessageBuilder(const uint16_t stream_id) : m_message() {
    m_message.m_stream_id = stream_id;
  };

  CQLMessageBuilder(const CQLMessage::Type type, const uint16_t stream_id)
      : CQLMessageBuilder(stream_id) {
    m_message.m_type = type;
  };

  CQLMessageBuilder(const CQLMessage::Type type, const uint16_t stream_id,
                    const CQLMessage::Opcode opcode)
      : CQLMessageBuilder(type, stream_id) {
    m_message.m_opcode = opcode;
  };

  CQLMessageBuilder(const CQLMessage::Type type, const uint16_t stream_id,
                    const CQLMessage::Opcode opcode, const std::string &body)
      : CQLMessageBuilder(type, stream_id, opcode) {
    m_message.m_body = body;
  }

  CQLMessage build() const {
    // TODO: Add assertions to check if all required fields are set
    return m_message;
  }

  std::string serialize() const { return build().serialize(); }

  CQLMessageBuilder &set_version(const uint8_t version) {
    m_message.m_version = version;
    return *this;
  }

  CQLMessageBuilder &set_flags(const uint8_t flags) {
    m_message.m_flags = flags;
    return *this;
  }

  CQLMessageBuilder &add_flag(const CQLMessage::Flag flag) {
    m_message.m_flags |= flag;
    return *this;
  }

  CQLMessageBuilder &set_stream_id(const uint16_t stream_id) {
    m_message.m_stream_id = stream_id;
    return *this;
  }

  CQLMessageBuilder &set_opcode(const CQLMessage::Opcode opcode) {
    m_message.m_opcode = opcode;
    return *this;
  }

  CQLMessageBuilder &set_body(const std::string &body) {
    m_message.m_body = body;
    return *this;
  }

  static CQLMessageBuilder from_ready(const uint16_t stream_id = 0x00) {
    return CQLMessageBuilder(CQLMessage::Type::Response, stream_id,
                             CQLMessage::Opcode::READY);
  }

  static CQLMessageBuilder from_supported(const uint16_t stream_id = 0x00) {
    std::string body =
        CQLSerializer::serialize_multimap({{"PROTOCOL_VERSIONS", "3/V3"},
                                           {"CQL_VERSION", "3.0.0"},
                                           {"COMPRESSION", "lz4"}});

    return CQLMessageBuilder(CQLMessage::Type::Response, stream_id,
                             CQLMessage::Opcode::SUPPORTED, body);
  }

  // TODO: support error messages
  static CQLMessageBuilder from_error(const uint16_t stream_id,
                                      const int32_t code,
                                      const std::string &message) {
    std::string body = CQLSerializer::serialize_int(code) +
                       CQLSerializer::serialize_string(message);

    return CQLMessageBuilder(CQLMessage::Type::Response, stream_id,
                             CQLMessage::Opcode::ERROR, body);
  }

  static CQLMessageBuilder from_result(const uint16_t stream_id = 0x00,
                                       const std::string &body = "") {
    return CQLMessageBuilder(
        CQLMessage::Type::Response, stream_id, CQLMessage::Opcode::RESULT,
        body == "" ? CQLSerializer::serialize_int(0x01) : body);
  }

private:
  CQLMessage m_message;
};

} // namespace cql
