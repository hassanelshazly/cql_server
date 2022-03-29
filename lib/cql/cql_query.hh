#pragma once

#include "common.hh"
#include "util/assert.hh"
namespace cql {

class  CQLQuery {
public:
  enum Consistency {
    ANY = 0x00,
    ONE = 0x01,
    TWO = 0x02,
    THREE = 0x03,
    QUORUM = 0x04,
    ALL = 0x05,
    LOCAL_QUORUM = 0x06,
    EACH_QUORUM = 0x07,
    SERIAL = 0x08,
    LOCAL_SERIAL = 0x09,
    LOCAL_ONE = 0x0A
  };

  CQLQuery()
      : m_consistency(ANY), m_flags(0), m_result_page_size(0),
        m_serial_consistency(ANY), m_timestamp(0) {}

  CQLQuery(const std::string &query, const Consistency consistency,
           const uint8_t flags)
      : m_query(query), m_consistency(consistency), m_flags(flags) {}

  friend class CQLQueryParser;

  uint8_t flags() const { return m_flags; }
  std::string query() const { return m_query; }
  Consistency consistency() const { return m_consistency; }

  bool values_flag() const { return m_flags & VALUES_FLAG; }
  bool skip_metadata_flag() const { return m_flags & SKIP_METADATA_FLAG; }
  bool page_size_flag() const { return m_flags & PAGE_SIZE_FLAG; }
  bool paging_state_flag() const { return m_flags & PAGING_STATE_FLAG; }
  bool serial_consistency_flag() const {
    return m_flags & SERIAL_CONSISTENCY_FLAG;
  }
  bool timestamp_flag() const { return m_flags & TIMESTAMP_FLAG; }
  bool names_for_values_flag() const { return m_flags & NAMES_FOR_VALUES_FLAG; }

  std::vector<std::string> values() const {
    assert(values_flag());
    return m_values;
  }
  std::vector<std::string> names_for_values() const {
    assert(names_for_values_flag());
    return m_names;
  }
  uint32_t result_page_size() const {
    assert(page_size_flag());
    return m_result_page_size;
  }
  std::string paging_state() const {
    assert(paging_state_flag());
    return m_paging_state;
  }
  Consistency serial_consistency() const {
    assert(serial_consistency_flag());
    return m_serial_consistency;
  }
  uint64_t timestamp() const {
    assert(timestamp_flag());
    return m_timestamp;
  }

  std::string to_string(bool include_params = false) const {
    static std::string consistencies[] = {
        "ANY",    "ONE",          "TWO",          "THREE",
        "QUORUM", "ALL",          "LOCAL_QUORUM", "EACH_QUORUM",
        "SERIAL", "LOCAL_SERIAL", "LOCAL_ONE"};

    std::stringstream ss;
    ss << "Query(" << m_query << ",\n"
       << "\tconsistency=" << consistencies[m_consistency] << ",\n";

    if (include_params) {
      ss << "\tflags ( values_flag=" << values_flag() << ",\n"
         << "\t\tskip_metadata_flag=" << skip_metadata_flag() << ",\n"
         << "\t\tpage_size_flag=" << page_size_flag() << ",\n"
         << "\t\tpaging_state_flag=" << paging_state_flag() << ",\n"
         << "\t\tserial_consistency_flag=" << serial_consistency_flag() << ",\n"
         << "\t\ttimestamp_flag=" << timestamp_flag() << ",\n"
         << "\t\tnames_for_values_flag=" << names_for_values_flag() << ")\n";

      if (values_flag()) {
        ss << "\tvalues=[";
        for (uint64_t i = 0; i < m_values.size(); ++i) {
          if (names_for_values_flag()) {
            ss << "(" << m_names[i] << ", " << m_values[i] << "), ";
          } else {
            ss << m_values[i] << ", ";
          }
        }
        ss << "]\n";
      }
      if (page_size_flag()) {
        ss << "\tresult_page_size=" << m_result_page_size << "\n";
      }
      if (paging_state_flag()) {
        ss << "\tpaging_state=" << m_paging_state << "\n";
      }
      if (serial_consistency_flag()) {
        ss << "\tserial_consistency=" << consistencies[m_serial_consistency]
           << "\n";
      }
      if (timestamp_flag()) {
        ss << "\ttimestamp=" << m_timestamp << "\n";
      }
      ss << ")";
    } else {
      ss << "\tflags=0x" << std::hex << static_cast<uint32_t>(m_flags) << ")\n";
    }
    return ss.str();
  };

  friend class CQLMessageBuilder;
  friend class CQLMessageParser;

private:
  std::string m_query;
  Consistency m_consistency;
  uint8_t m_flags;

  std::vector<std::string> m_names;
  std::vector<std::string> m_values;

  uint32_t m_result_page_size;
  std::string m_paging_state;
  Consistency m_serial_consistency;
  uint64_t m_timestamp;

  static constexpr uint8_t VALUES_FLAG = 0x01;
  static constexpr uint8_t SKIP_METADATA_FLAG = 0x02;
  static constexpr uint8_t PAGE_SIZE_FLAG = 0x04;
  static constexpr uint8_t PAGING_STATE_FLAG = 0x08;
  static constexpr uint8_t SERIAL_CONSISTENCY_FLAG = 0x10;
  static constexpr uint8_t TIMESTAMP_FLAG = 0x20;
  static constexpr uint8_t NAMES_FOR_VALUES_FLAG = 0x40;
};

} // namespace cql
