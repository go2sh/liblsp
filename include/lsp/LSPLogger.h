#ifndef LSP_LSPLOGGER_H
#define LSP_LSPLOGGER_H

#include <lsp/LanguageClient.h>
#include <lsp/Logger.h>

namespace lsp {
class LspLogger : public Logger {
  LanguageClient *Client;

public:
  LspLogger(LanguageClient *Client) : Client(Client) {}
  /// Log an error message
  virtual void error(const std::string &Message) const {
    Client->logMessage({lsp::LogMessageParams::MessageType::Error, Message});
  }

  /// Log an info message
  virtual void info(const std::string &Message) const {
    Client->logMessage({lsp::LogMessageParams::MessageType::Info, Message});
  }

  /// Log a warning message
  virtual void warning(const std::string &Message) const {
    Client->logMessage({lsp::LogMessageParams::MessageType::Warning, Message});
  }

  /// Log a generic message
  virtual void log(const std::string &Message) const {
    Client->logMessage({lsp::LogMessageParams::MessageType::Log, Message});
  }
};
} // namespace lsp

#endif // !LSP_LSPLOGGER_H
