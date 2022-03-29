#pragma once

#include "common.hh"
#include "cql_option.hh"
#include "cql_result.hh"

namespace cql {

class  CQLResultBuilder {
public:
  CQLResultBuilder() = default;

  // TODO: Add prepared builders
  // TODO: review the code aginst the protocol

  CQLResult build() const {
    // TODO: Add assertions to check if all required fields are set
    return m_result;
  }

  std::string serialize() const { return build().serialize(); }

  CQLResultBuilder &set_kind(const CQLResult::Kind kind) {
    m_result.m_kind = kind;
    return *this;
  }

  CQLResultBuilder &
  set_rows(const std::vector<std::vector<std::string>> &rows) {
    m_result.data.rows.values.rows = rows;
    m_result.data.rows.values.rows_count = rows.size();
    return *this;
  }

  CQLResultBuilder &add_row(const std::vector<std::string> &row) {
    m_result.data.rows.values.rows.push_back(row);
    m_result.data.rows.values.rows_count++;
    return *this;
  }

  CQLResultBuilder &set_columns_specs(
      const std::vector<std::pair<std::string, CQLOption>> &columns) {
    m_result.data.rows.metadata.cols_specs = columns;
    m_result.data.rows.metadata.cols_count = columns.size();
    m_result.data.rows.metadata.flags &= ~CQLResult::NO_METADATA_FLAG;
    return *this;
  }

  CQLResultBuilder &
  add_column_spec(const std::pair<std::string, CQLOption> &spec) {
    m_result.data.rows.metadata.cols_specs.push_back(spec);
    m_result.data.rows.metadata.cols_count++;
    m_result.data.rows.metadata.flags &= ~CQLResult::NO_METADATA_FLAG;
    return *this;
  }

  CQLResultBuilder &clear_specs() {
    m_result.data.rows.metadata.cols_specs.clear();
    m_result.data.rows.metadata.local_table_specs.clear();
    m_result.data.rows.metadata.flags |= CQLResult::NO_METADATA_FLAG;
    return *this;
  }

  CQLResultBuilder &set_cols_count(const int32_t cols_count) {
    m_result.data.rows.metadata.cols_count = cols_count;
    return *this;
  }

  CQLResultBuilder &set_rows_count(const int32_t rows_count) {
    m_result.data.rows.values.rows_count = rows_count;
    return *this;
  }

  CQLResultBuilder &set_paging_state(const std::string &paging_state) {
    m_result.data.rows.metadata.paging_state = paging_state;
    m_result.data.rows.metadata.flags |= CQLResult::HAS_MORE_PAGES_FLAG;
    return *this;
  }

  CQLResultBuilder &clear_paging_state() {
    m_result.data.rows.metadata.flags &= ~CQLResult::HAS_MORE_PAGES_FLAG;
    return *this;
  }

  CQLResultBuilder &
  set_global_table_spec(const std::pair<std::string, std::string> &table_spec) {
    m_result.data.rows.metadata.global_table_spec = table_spec;
    m_result.data.rows.metadata.flags |= CQLResult::GLOBAL_SPEC_FLAG;
    m_result.data.rows.metadata.flags &= ~CQLResult::NO_METADATA_FLAG;
    return *this;
  }

  CQLResultBuilder &set_local_table_specs(
      const std::vector<std::pair<std::string, std::string>> &table_spec) {
    m_result.data.rows.metadata.local_table_specs = table_spec;
    m_result.data.rows.metadata.flags &= ~CQLResult::GLOBAL_SPEC_FLAG;
    m_result.data.rows.metadata.flags &= ~CQLResult::NO_METADATA_FLAG;
    return *this;
  }

  CQLResultBuilder &set_keyspace(const std::string &keyspace) {
    m_result.data.keyspace = keyspace;
    return *this;
  }

  CQLResultBuilder &set_prepared_id(const std::string &id) {
    m_result.data.prepared.id = id;
    return *this;
  }

  CQLResultBuilder &set_schema_change_type(const std::string &type) {
    m_result.data.schema_change.type = type;
    return *this;
  }

  CQLResultBuilder &set_schema_change_target(const std::string &target) {
    m_result.data.schema_change.target = target;
    return *this;
  }

  CQLResultBuilder &set_schema_change_keyspace(const std::string &keyspace) {
    m_result.data.schema_change.keyspace = keyspace;
    return *this;
  }

  CQLResultBuilder &set_schema_change_object(const std::string &object) {
    m_result.data.schema_change.object = object;
    return *this;
  }

  static CQLResultBuilder from_void() {
    return CQLResultBuilder().set_kind(CQLResult::Kind::VOID);
  }

  static CQLResultBuilder from_rows() {
    return CQLResultBuilder().set_kind(CQLResult::Kind::ROWS);
  }

  static CQLResultBuilder from_set_keyspace(std::string keyspace = "") {
    return CQLResultBuilder()
        .set_kind(CQLResult::Kind::SET_KEYSPACE)
        .set_keyspace(keyspace);
  }

  static CQLResultBuilder from_prepared(std::string id = "") {
    return CQLResultBuilder()
        .set_kind(CQLResult::Kind::PREPARED)
        .set_prepared_id(id);
  }

  static CQLResultBuilder from_schema_change(std::string type = "",
                                             std::string target = "",
                                             std::string keyspace = "",
                                             std::string object = "") {
    return CQLResultBuilder()
        .set_kind(CQLResult::Kind::SCHEMA_CHANGE)
        .set_schema_change_type(type)
        .set_schema_change_target(target)
        .set_schema_change_keyspace(keyspace)
        .set_schema_change_object(object);
  }

  static CQLResultBuilder demo() {
    CQLResultBuilder result_builder;
    CQLOption option;
    std::vector<std::pair<std::string, CQLOption>> cols_spec = {
        {"key", option},
        {"cluster_name", option},
        {"cql_version", option},
        {"release_version", option},
        {"native_protocol_version", option},
        {"keyspace_name", option},
        {"table_name", option},
        {"view_name", option},
        {"column_name", option},
        {"base_table_name", option},
        {"include_all_columns", option},
        {"where_clause", option},
        {"type", option},
        {"clustering_order", option}};
    std::vector<std::string> row = {"local",  "DiBibase Cluster",
                                    "3.0.0",  "3.1.1",
                                    "3.2.2",  "keyspace",
                                    "table",  "view",
                                    "column", "base_table",
                                    "true",   "true",
                                    "false",  "asc"};
    result_builder.set_kind(CQLResult::Kind::ROWS);
    result_builder.set_global_table_spec({"system", "local"});
    result_builder.set_columns_specs(cols_spec);
    result_builder.set_rows({row});
    return result_builder;
  }

private:
  CQLResult m_result;
};

} // namespace cql
