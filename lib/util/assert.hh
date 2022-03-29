#pragma once

#include "logger.hh"

namespace cql::util {

#define ABORT_ON_FAILURE

#define STRINGFING(x) #x
#define STR(x) STRINGFING(x)
#define FILE_LINE __FILE__ ":" STR(__LINE__) ":in "

#define fail_0() util::fail_m(std::string() + FILE_LINE + __func__ + "()")

#define fail_1(message)                                                        \
  util::fail_m(std::string() + FILE_LINE + __func__ + "(): " + message)

#define assert_1(condition)                                                    \
  util::assert_m(condition, std::string() + FILE_LINE + __func__ + "()")

#define assert_2(condition, message)                                           \
  util::assert_m(condition,                                                    \
                 std::string() + FILE_LINE + __func__ + "(): " + message)

#define assert_eq_2(ele_1, ele_2)                                              \
  util::assert_eq_m(ele_1, ele_2, std::string() + FILE_LINE + __func__ + "()")

#define assert_eq_3(ele_1, ele_2, message)                                     \
  util::assert_eq_m(ele_1, ele_2,                                              \
                    std::string() + FILE_LINE + __func__ + "(): " + message)

#define GET_2ND_ARG(arg1, arg2, ...) arg2

#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3

#define GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4

#define FAIL_MACRO_CHOOSER(...) GET_2ND_ARG(__VA_ARGS__, fail_1, fail_0)

#define ASSERT_MACRO_CHOOSER(...) GET_3RD_ARG(__VA_ARGS__, assert_2, assert_1, )

#define ASSERT_EQ_MACRO_CHOOSER(...)                                           \
  GET_4TH_ARG(__VA_ARGS__, assert_eq_3, assert_eq_2, , )

#define fail(...) FAIL_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define assert(...) ASSERT_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define assert_eq(...) ASSERT_EQ_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

bool assert_m(bool condition, const std::string &message = "") {
  static Logger logger = Logger::make();

  if (!condition) {
    logger.err("Assertion failed: %s", message.c_str());
#ifdef ABORT_ON_FAILURE
    abort();
#endif
  }

  return condition;
}

template <typename type_t>
bool assert_eq_m(type_t ele_1, type_t ele_2, const std::string &message = "") {
  static Logger logger = Logger::make();
  std::stringstream ss;

  bool vaild = ele_1 == ele_2;
  if (!vaild) {
    ss << "Assertion failed: " << message << "\n\t";
    ss << "Expected: `" << ele_1 << "` but got: `" << ele_2 << "`";
    logger.err(ss.str());
#ifdef ABORT_ON_FAILURE
    abort();
#endif
  }

  return vaild;
}

bool fail_m(const std::string &message = "") {
  static Logger logger = Logger::make();

  logger.err("Assertion failed: %s", message.c_str());
#ifdef ABORT_ON_FAILURE
  abort();
#endif

  return false;
}

} // namespace cql::util
