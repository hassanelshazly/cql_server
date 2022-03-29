#pragma once

#include "common.hh"
#include "cql_option.hh"
#include "cql_serializer.hh"
#include "util/assert.hh"

namespace cql {

// TODO: Option class is too complex for udt and tuples
//       (maybe we will need option_builder)
class  CQLOption {
public:
  CQLOption() : m_id(Id::VARCHAR){};

  enum Id {
    CUSTOM = 0x00,
    ASCII = 0x01,
    BIGINT = 0x02,
    BLOB = 0x03,
    BOOLEAN = 0x04,
    COUNTER = 0x05,
    DECIMAL = 0x06,
    DOUBLE = 0x07,
    FLOAT = 0x08,
    INT = 0x09,
    TIMESTAMP = 0x0B,
    UUID = 0x0C,
    VARCHAR = 0x0D,
    VARINT = 0x0E,
    TIMEUUID = 0x0F,
    INET = 0x10,
    LIST = 0x20,
    MAP = 0x21,
    SET = 0x22,
    UDT = 0x30,
    TUPLE = 0x31
  };

  CQLOption(const Id id) : m_id(id){};

  Id id() const { return m_id; }

  CQLOption option() const {
    if (!(m_id == Id::MAP || m_id == Id::LIST || m_id == Id::SET)) {
      fail("options are not compatible");
    }
    return *value.option;
  }
  std::string custom() const {
    assert_eq(m_id, Id::CUSTOM);
    return value.custom;
  }
  std::vector<CQLOption> tuple_options() const {
    assert_eq(m_id, Id::TUPLE);
    return value.tuple.options;
  }
  std::string udt_name() const {
    assert_eq(m_id, Id::UDT);
    return value.udt.name;
  }
  std::string udt_keyspace() const {
    assert_eq(m_id, Id::UDT);
    return value.udt.keyspace;
  }
  std::vector<std::pair<std::string, CQLOption>> udt_specs() const {
    assert_eq(m_id, Id::UDT);
    return value.udt.specs;
  }

  std::string serialize() const {
    std::stringstream ss;
    ss << CQLSerializer::serialize_short(m_id);
    switch (m_id) {
    case CUSTOM:
      ss << CQLSerializer::serialize_string(value.custom);
      break;
    case ASCII:
    case BIGINT:
    case BLOB:
    case BOOLEAN:
    case COUNTER:
    case DECIMAL:
    case DOUBLE:
    case FLOAT:
    case INT:
    case TIMESTAMP:
    case UUID:
    case VARCHAR:
    case VARINT:
    case TIMEUUID:
    case INET:
      break;
    case LIST:
    case MAP:
    case SET:
      ss << value.option->serialize();
      break;
    case UDT:
      serialize_udt(ss);
      break;
    case TUPLE:
      serialize_tuple(ss);
      break;
    default:
      fail("Unknown option");
      break;
    }
    return ss.str();
  }

  friend class CQLResultBuilder;

private:
  std::stringstream &serialize_tuple(std::stringstream &ss) const {
    ss << CQLSerializer::serialize_short(value.tuple.count);
    for (int32_t i = 0; i < value.tuple.count; ++i) {
      ss << value.tuple.options[i].serialize();
    }
    return ss;
  }

  std::stringstream &serialize_udt(std::stringstream &ss) const {
    ss << CQLSerializer::serialize_string(value.udt.keyspace);
    ss << CQLSerializer::serialize_string(value.udt.name);
    ss << CQLSerializer::serialize_short(value.udt.count);

    for (int32_t i = 0; i < value.tuple.count; ++i) {
      auto &[name, option] = value.udt.specs[i];
      ss << CQLSerializer::serialize_string(name);
      ss << option.serialize();
    }
    return ss;
  }

private:
  Id m_id;
  // TODO: use union or variant
  struct value_t {
    std::shared_ptr<CQLOption> option;
    std::string custom;
    struct tuple_t {
      tuple_t() : count(0){};

      int32_t count;
      std::vector<CQLOption> options;
    } tuple;
    struct udt_t {
      udt_t() : count(0){};
      std::string keyspace;
      std::string name;
      uint16_t count;
      std::vector<std::pair<std::string, CQLOption>> specs;
    } udt;
  } value;
};

} // namespace cql
