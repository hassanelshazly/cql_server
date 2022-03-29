#pragma once

#include "common.hh"

namespace cql {

class  CQLMessage {
public:
  enum Flag { NONE = 0x00, COMPRESSED = 0x01, TRACING = 0x02 };

  enum Opcode {
    ERROR = 0x00,
    STARTUP = 0x01,
    READY = 0x02,
    AUTHENTICATE = 0x03,
    OPTIONS = 0x05,
    SUPPORTED = 0x06,
    QUERY = 0x07,
    RESULT = 0x08,
    PREPARE = 0x09,
    EXECUTE = 0x0A,
    REGISTER = 0x0B,
    EVENT = 0x0C,
    BATCH = 0x0D,
    AUTH_CHALLENGE = 0x0E,
    AUTH_RESPONSE = 0x0F,
    AUTH_SUCCESS = 0x10
  };

  enum Type { Request = 0x0, Response = 0x1 };

  CQLMessage()
      : m_type(Response), m_version(VERSION), m_flags(0x00), m_stream_id(0x00),
        m_opcode(ERROR), m_body(std::string()) {}

  CQLMessage(const Type type, const uint8_t version, const uint8_t flags,
             const uint16_t stream_id, const Opcode opcode,
             const std::string &body)
      : m_type(type), m_version(version), m_flags(flags),
        m_stream_id(stream_id), m_opcode(opcode), m_body(body) {}

  uint8_t flags() const { return m_flags; }
  uint16_t stream_id() const { return m_stream_id; }
  Opcode opcode() const { return m_opcode; }
  Type type() const { return m_type; }
  std::string body() const { return m_body; }
  uint32_t length() const { return m_body.length(); }
  uint8_t version() const { return m_version; }

  std::string to_string() const {
    static std::string types[] = {"Request", "Response"};
    static std::string opcodes[] = {
        "ERROR",           "STARTUP", "READY",          "AUTHENTICATE",
        "NOT_IMPLEMENTED", "OPTIONS", "SUPPORTED",      "QUERY",
        "RESULT",          "PREPARE", "EXECUTE",        "REGISTER",
        "EVENT",           "BATCH",   "AUTH_CHALLENGE", "AUTH_RESPONSE",
        "AUTH_SUCCESS"};

    std::stringstream ss;
    ss << "CQLMessage(type=" << types[m_type] << ", version=0x" << std::hex
       << static_cast<uint32_t>(m_version) << ", flags=0x"
       << static_cast<uint32_t>(m_flags) << ", stream_id=" << std::dec
       << m_stream_id << ", opcode=" << opcodes[m_opcode]
       << ", body_length=" << m_body.length() << ")";
    return ss.str();
  };

  std::string header_view() const {
    std::string header = serialize();
    std::stringstream ss;
    ss << std::hex << "Header(";
    for (size_t i = 0; i < HEADER_LENGTH - 1; ++i) {
      ss << "0x" << static_cast<uint32_t>(header[i]) << ", ";
    }
    ss << "0x" << static_cast<uint32_t>(header[HEADER_LENGTH - 1]) << ")";
    return ss.str();
  }

  std::string serialize() const {
    // TODO: check vadility in big endian machines
    std::string binary;
    binary.reserve(CQLMessage::HEADER_LENGTH + m_body.length() + 1);
    binary.resize(CQLMessage::HEADER_LENGTH);
    binary[0] = m_type << 0x7 | m_version;
    binary[1] = m_flags;
    binary[2] = m_stream_id >> 0x8 & 0xFF;
    binary[3] = m_stream_id >> 0x0 & 0xFF;
    binary[4] = m_opcode;
    binary[5] = m_body.length() >> 0x18 & 0xFF;
    binary[6] = m_body.length() >> 0x10 & 0xFF;
    binary[7] = m_body.length() >> 0x08 & 0xFF;
    binary[8] = m_body.length() >> 0x00 & 0xFF;
    binary.append(m_body);
    return binary;
  }

  friend class CQLMessageBuilder;
  friend class CQLMessageParser;

public:
  static constexpr uint8_t HEADER_LENGTH = 9;

private:
  Type m_type;
  uint8_t m_version;
  uint8_t m_flags;
  uint16_t m_stream_id;
  Opcode m_opcode;
  std::string m_body;

  static constexpr uint8_t VERSION = 0x03;
};

} // namespace cql
