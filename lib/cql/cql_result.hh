#pragma once

#include "common.hh"
#include "cql_option.hh"
#include "cql_serializer.hh"

namespace cql {

#define KYESPACE_TARGET "KEYSPACE"

class  CQLResult {
public:
  enum Kind {
    VOID = 0x01,
    ROWS = 0x02,
    SET_KEYSPACE = 0x03,
    PREPARED = 0x04,
    SCHEMA_CHANGE = 0x05
  };

  CQLResult() : m_kind(Kind::VOID){};

  // TODO: add getters and assertions for types
  Kind kind() const { return m_kind; }
  int32_t flags() const { return data.rows.metadata.flags; };
  int32_t rows_count() const { return data.rows.values.rows_count; }
  int32_t cols_count() const { return data.rows.metadata.cols_count; }

  bool global_table_spec_flag() const {
    return data.rows.metadata.flags & GLOBAL_SPEC_FLAG;
  }
  bool has_more_pages_flag() const {
    return data.rows.metadata.flags & HAS_MORE_PAGES_FLAG;
  }
  bool no_metadata_flag() const {
    return data.rows.metadata.flags & NO_METADATA_FLAG;
  }

  std::string to_string() const {
    fail("not implemented");
    return "";
  }

  std::string serialize() const {
    switch (m_kind) {
    case VOID:
      return CQLSerializer::serialize_int(m_kind);
    case ROWS:
      return serialize_rows();
    case SET_KEYSPACE:
      return serialize_set_keyspace();
    case PREPARED:
      return serialize_prepared();
    case SCHEMA_CHANGE:
      return serialize_schema_change();
    default:
      fail("Unknown kind");
      return "";
    }
  }

  friend class CQLResultBuilder;

private:
  struct metadata_t {
    metadata_t() : flags(0), cols_count(0) {}
    int32_t flags;
    int32_t cols_count;
    std::string paging_state;
    std::pair<std::string, std::string> global_table_spec;
    std::vector<std::pair<std::string, std::string>> local_table_specs;
    std::vector<std::pair<std::string, CQLOption>> cols_specs;
  };

  Kind m_kind;
  // TODO: use union or variant
  struct data_t {
    struct rows_t {
      metadata_t metadata;
      struct values_t {
        values_t() : rows_count(0) {}

        int32_t rows_count;
        std::vector<std::vector<std::string>> rows;
      } values;
    } rows;

    std::string keyspace;

    struct prepared_t {
      std::string id;
      metadata_t metadata;
      metadata_t result_metadata;
    } prepared;

    struct schema_change_t {
      std::string type;
      std::string target;
      std::string keyspace;
      std::string object;
    } schema_change;
  } data;

  static constexpr int32_t GLOBAL_SPEC_FLAG = 0x01;
  static constexpr int32_t HAS_MORE_PAGES_FLAG = 0x02;
  static constexpr int32_t NO_METADATA_FLAG = 0x04;

private:
  std::string serialize_rows() const {
    std::stringstream ss;

    ss << CQLSerializer::serialize_int(m_kind);

    serialize_metadata(ss, data.rows.metadata);

    ss << CQLSerializer::serialize_int(data.rows.values.rows_count);
    for (int32_t i = 0; i < data.rows.values.rows_count; ++i) {
      for (int32_t j = 0; j < data.rows.metadata.cols_count; ++j) {
        ss << CQLSerializer::serialize_bytes(data.rows.values.rows[i][j]);
      }
    }

    return ss.str();
  }

  std::string serialize_set_keyspace() const {
    std::stringstream ss;

    ss << CQLSerializer::serialize_int(m_kind);
    ss << CQLSerializer::serialize_string(data.keyspace);

    return ss.str();
  }

  std::string serialize_prepared() const {
    std::stringstream ss;

    ss << CQLSerializer::serialize_int(m_kind);
    ss << CQLSerializer::serialize_short_bytes(data.prepared.id);
    serialize_metadata(ss, data.prepared.metadata);
    serialize_metadata(ss, data.prepared.result_metadata);
    return ss.str();
  }

  std::string serialize_schema_change() const {
    std::stringstream ss;

    ss << CQLSerializer::serialize_int(m_kind);

    auto &[type, target, keyspace, object] = data.schema_change;
    ss << CQLSerializer::serialize_string(type);
    ss << CQLSerializer::serialize_string(target);
    ss << CQLSerializer::serialize_string(keyspace);

    if (target != KYESPACE_TARGET) {
      ss << CQLSerializer::serialize_string(object);
    }

    return ss.str();
  }

  std::stringstream &serialize_metadata(std::stringstream &ss,
                                        const metadata_t &metadata) const {
    ss << CQLSerializer::serialize_int(metadata.flags);
    ss << CQLSerializer::serialize_int(metadata.cols_count);

    if (has_more_pages_flag()) {
      ss << CQLSerializer::serialize_bytes(metadata.paging_state);
    }

    if (!no_metadata_flag()) {
      if (global_table_spec_flag()) {
        auto &[keyspace, table] = metadata.global_table_spec;
        ss << CQLSerializer::serialize_string(keyspace);
        ss << CQLSerializer::serialize_string(table);
      }
      for (int32_t i = 0; i < metadata.cols_count; ++i) {
        if (!global_table_spec_flag()) {
          auto &[keyspace, table] = metadata.local_table_specs[i];
          ss << CQLSerializer::serialize_string(keyspace);
          ss << CQLSerializer::serialize_string(table);
        }
        auto &[col, option] = metadata.cols_specs[i];
        ss << CQLSerializer::serialize_string(col);
        ss << option.serialize();
      }
    }
    return ss;
  }
};

} // namespace cql
