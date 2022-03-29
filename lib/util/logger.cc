#include "logger.hh"

#include <unistd.h>

#include "errors.hh"
#include "util/factory.hh"

namespace cql::util {

const char *Logger::convert(const string &arg) const { return arg.c_str(); }

const Logger &Logger::make(LogLevel level) {
  return *get_singleton<std::remove_reference_t<const Logger &>>(level);
}

Logger::Logger(LogLevel level) : m_level(level) {
  // clang-format off
  if (isatty(m_fd)) {
    m_prefixes[LogLevel::INFO]    = "\r\033[1;32m* \033[0m";
    m_prefixes[LogLevel::NOTICE]  = "\r\033[1;34mnotice: \033[0m";
    m_prefixes[LogLevel::WARNING] = "\r\033[1;33mwarn: \033[0m";
    m_prefixes[LogLevel::ERROR]   = "\r\033[1;31merror: \033[0m";
    m_suffixes[LogLevel::INFO]    = "\033[0m";
    m_suffixes[LogLevel::NOTICE]  = "\033[0m";
    m_suffixes[LogLevel::WARNING] = "\033[0m";
    m_suffixes[LogLevel::ERROR]   = "\033[0m";
  } else {
    m_prefixes[LogLevel::INFO]    = "cql|info: ";
    m_prefixes[LogLevel::NOTICE]  = "cql|notice: ";
    m_prefixes[LogLevel::WARNING] = "cql|warn: ";
    m_prefixes[LogLevel::ERROR]   = "cql|error: ";
    m_suffixes[LogLevel::INFO]    = "";
    m_suffixes[LogLevel::NOTICE]  = "";
    m_suffixes[LogLevel::WARNING] = "";
    m_suffixes[LogLevel::ERROR]   = "";
  }
  // clang-format on
}

void Logger::set_verbosity(LogLevel level) { m_level = level; }

LogLevel Logger::parse_verbosity(const string &name, LogLevel fallback) {
  if (name == "error") {
    return LogLevel::ERROR;
  } else if (name == "warning") {
    return LogLevel::WARNING;
  } else if (name == "notice") {
    return LogLevel::NOTICE;
  } else if (name == "info") {
    return LogLevel::INFO;
  } else {
    return fallback;
  }
}

} // namespace cql::util
