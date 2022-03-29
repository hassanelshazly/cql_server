#pragma once

#include "common.hh"

namespace cql {

class CQLParser {
public:
  CQLParser(const std::string &data)
      : m_data(data), idx(0), logger(Logger::make()) {}

  uint64_t size() const { return m_data.size(); }
  std::string data() const { return m_data; }
  std::string remaining() const { return m_data.substr(idx); }
  uint64_t remaining_size() const { return m_data.size() - idx; }

protected:
  uint64_t parse_long() {
    uint64_t value = static_cast<uint64_t>(m_data[idx + 7]) << 0x38 |
                     static_cast<uint64_t>(m_data[idx + 6]) << 0x30 |
                     static_cast<uint64_t>(m_data[idx + 5]) << 0x28 |
                     static_cast<uint64_t>(m_data[idx + 4]) << 0x20 |
                     static_cast<uint64_t>(m_data[idx + 3]) << 0x18 |
                     static_cast<uint64_t>(m_data[idx + 2]) << 0x10 |
                     static_cast<uint64_t>(m_data[idx + 1]) << 0x08 |
                     static_cast<uint64_t>(m_data[idx + 0]) << 0x00;
    idx += 8;
    return byteswap64(value);
  }

  uint32_t parse_int() {
    uint32_t value = m_data[idx + 3] << 0x18 | m_data[idx + 2] << 0x10 |
                     m_data[idx + 1] << 0x08 | m_data[idx + 0] << 0x00;
    idx += 4;
    return byteswap32(value);
  }

  uint16_t parse_short() {
    uint16_t value = m_data[idx + 1] << 0x8 | m_data[idx];
    idx += 2;
    return byteswap16(value);
  }

  std::string parse_string() {
    uint16_t length = parse_short();
    std::string value = m_data.substr(idx, length);
    idx += length;
    return value;
  }

  std::string parse_bytes() {
    uint32_t length = parse_int();
    std::string value = m_data.substr(idx, length);
    idx += length;
    return value;
  }

  uint8_t parse_byte() {
    uint8_t value = m_data[idx];
    idx++;
    return value;
  }

protected:
  std::string m_data;
  uint64_t idx = 0;
  Logger logger;
};

} // namespace cql
