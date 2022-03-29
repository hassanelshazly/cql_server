#pragma once

#include "common.hh"
#include "cql_message.hh"

namespace cql {

class CQLSerializer {
public:
  static std::string serialize_long(int64_t value) {
    value = byteswap64(value);
    return std::string(reinterpret_cast<const char *>(&value), sizeof(value));
  }

  static std::string serialize_int(int32_t value) {
    value = byteswap32(value);
    return std::string(reinterpret_cast<const char *>(&value), sizeof(value));
  }

  static std::string serialize_short(uint16_t value) {
    value = byteswap16(value);
    return std::string(reinterpret_cast<const char *>(&value), sizeof(value));
  }

  static std::string serialize_string(const std::string &value) {
    // TODO: support UTF-8
    return serialize_short(value.length()) + value;
  }

  static std::string serialize_long_string(const std::string &value) {
    // TODO: support UTF-8
    return serialize_int(value.length()) + value;
  }

  static std::string serialize_bytes(const std::string &value) {
    return serialize_int(value.length()) + value;
  }

  static std::string serialize_short_bytes(const std::string &value) {
    return serialize_short(value.length()) + value;
  }

  static std::string serialize_null() { return serialize_int(-1); }

  static std::string
  serialize_string_list(const std::vector<std::string> &values) {
    std::string result = serialize_short(values.size());

    for (const auto &value : values) {
      result += serialize_string(value);
    }
    return result;
  }

  static std::string
  serialize_map(const std::map<std::string, std::string> &values) {
    std::string result = serialize_short(values.size());

    for (const auto &value : values) {
      result += serialize_string(value.first) + serialize_string(value.second);
    }
    return result;
  }

  static std::string serialize_multimap(
      const std::map<std::string, std::vector<std::string>> &values) {
    std::string result = serialize_short(values.size());

    for (const auto &value : values) {
      result +=
          serialize_string(value.first) + serialize_string_list(value.second);
    }
    return result;
  }

  static std::string
  serialize_multimap(const std::multimap<std::string, std::string> &values) {
    std::map<std::string, std::vector<std::string>> mmap;

    for (const auto &value : values) {
      mmap[value.first].push_back(value.second);
    }
    return serialize_multimap(mmap);
  }

  // TODO: add support for serializing CQL types uuid, blob, inet, etc.
};

} // namespace cql
