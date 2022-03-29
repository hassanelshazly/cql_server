#pragma once

#include "common.hh"
#include "cql_query.hh"
#include "cql_query_parser.hh"
#include "cql_result.hh"
#include "cql_result_builder.hh"

namespace cql {

class  CQLQueryProcessor {
public:
  // TODO: implement constructors
  // TODO: implement the whole class :)
  // TODO: (what about using cql_query_handler as a wrapper ?)
  CQLQueryProcessor(const CQLQuery &query)
      : m_query(query), logger(Logger::make()) {}

  CQLQueryProcessor(const std::string &query)
      : CQLQueryProcessor(CQLQueryParser(query).parse()) {}

  CQLQuery query() { return m_query; }

  CQLResult process() {
    logger.info(m_query.to_string());
    return CQLResultBuilder::demo().build();
  }

  std::string serialize() { return process().serialize(); }

private:
  const CQLQuery m_query;
  Logger logger;
};

} // namespace cql
