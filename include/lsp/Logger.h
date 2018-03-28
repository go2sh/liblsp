#ifndef LSP_LOGGER_H
#define LSP_LOGGER_H

#include <string>

namespace lsp {

/// Generic logger
/// The class is handling the logging of messages with different types
class Logger {
public:
  /// Log an error message
  virtual void error(const std::string &Message) const = 0;

  /// Log an info message
  virtual void info(const std::string &Message) const = 0;

  /// Log a warning message
  virtual void warning(const std::string &Message) const = 0;

  /// Log a generic message
  virtual void log(const std::string &Message) const = 0;
};

class NullLogger : public Logger {
public:
  /// Log an error message
  virtual void error(const std::string &Message) const {}

  /// Log an info message
  virtual void info(const std::string &Message) const  {}

  /// Log a warning message
  virtual void warning(const std::string &Message) const  {}

  /// Log a generic message
  virtual void log(const std::string &Message) const  {}
};
}
#endif