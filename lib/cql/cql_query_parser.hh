#pragma once

#include "common.hh"
#include "cql_parser.hh"
#include "cql_query.hh"

namespace cql {

class  CQLQueryParser : public CQLParser {
public:
  CQLQueryParser(const std::string &data) : CQLParser(data) {}

  CQLQuery parse() {
    CQLQuery query;
    query.m_query = parse_bytes();
    query.m_consistency = parse_consistency();
    query.m_flags = parse_byte();

    if (query.values_flag()) {
      auto [names, values] =
          parse_names_and_values(query.names_for_values_flag());

      query.m_names = std::move(names);
      query.m_values = std::move(values);
    }
    if (query.page_size_flag()) {
      query.m_result_page_size = parse_int();
    }
    if (query.paging_state_flag()) {
      query.m_paging_state = parse_bytes();
    }
    if (query.serial_consistency_flag()) {
      query.m_serial_consistency = parse_consistency();
    }
    if (query.timestamp_flag()) {
      query.m_timestamp = parse_long();
    }
    return query;
  }

private:
  CQLQuery::Consistency parse_consistency() {
    return static_cast<CQLQuery::Consistency>(parse_short());
  }

  std::pair<std::vector<std::string>, std::vector<std::string>>
  parse_names_and_values(bool names_for_values = false) {
    uint32_t length = parse_int();
    std::vector<std::string> values(length);
    std::vector<std::string> names(length);
    for (uint32_t i = 0; i < length; i++) {
      if (names_for_values) {
        names[i] = parse_string();
      }
      values[i] = parse_string();
    }
    return {names, values};
  }
};

} // namespace cql
