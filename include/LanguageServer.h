#ifndef LSP_LANGUAGESERVER_H
#define LSP_LANGUAGESERVER_H

#include "LSProtocol.h"
#include "MessageConnection.h"

namespace lsp {
class LanguageServer {

public:
  void connect(MessageConnection &Connection) {
    Connection.registerHandler(
        "initialize", std::bind(&LanguageServer::handleInitialize, this,
                                std::placeholders::_1, std::placeholders::_2));
    Connection.registerHandler(
        "shutdown", std::bind(&LanguageServer::handleShutdown, this,
                              std::placeholders::_1, std::placeholders::_2));
    Connection.registerHandler("exit", std::bind(&LanguageServer::handleExit,
                                                 this, std::placeholders::_1,
                                                 std::placeholders::_2));
    Connection.registerHandler("textDocument/hover",
                               std::bind(&LanguageServer::handleHover, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
    Connection.registerHandler("textDocument/completion",
                               std::bind(&LanguageServer::handleCompletion,
                                         this, std::placeholders::_1,
                                         std::placeholders::_2));
    Connection.registerHandler(
        "textDocument/didChange",
        std::bind(&LanguageServer::handleTextDocumentDidChange, this,
                  std::placeholders::_1, std::placeholders::_2));
  }

  virtual InitializeResult
  initialize(const InitializeParams<EmptyInitializationOptions> &Params) = 0;
  virtual Hover hover(const TextDocumentPositionParams &Params) = 0;
  virtual std::vector<CompletionItem<EmptyCompletionData>>
  completion(const CompletionParams &Params) = 0;
  virtual void
  textDocumentDidChange(const TextDocumentDidChangeParams &Params) = 0;

  virtual void shutdown() = 0;
  virtual void exit() = 0;

protected:
  void handleInitialize(const json &Params, json &Result);
  void handleShutdown(const json &Params, json &Result);
  void handleExit(const json &Params, json &Result);
  void handleHover(const json &Params, json &Result);
  void handleCompletion(const json &Params, json &Result);
  void handleTextDocumentDidChange(const json &Params, json &Result);
};

}; // namespace lsp
#endif