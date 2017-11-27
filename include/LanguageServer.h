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
    Connection.registerHandler("textDocument/hover",
                               std::bind(&LanguageServer::handleHover, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
    Connection.registerHandler("textDocument/completion",
                               std::bind(&LanguageServer::handleCompletion, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }

  virtual InitializeResult
  initialize(const InitializeParams<EmptyInitializationOptions> &Params) = 0;
  virtual Hover hover(const TextDocumentPositionParams &Params) = 0;
  virtual std::vector<CompletionItem<EmptyCompletionData>>
  completion(const CompletionParams &Params) = 0;

protected:
  void handleInitialize(const json &Params, json &Result);
  void handleHover(const json &Params, json &Result);
  void handleCompletion(const json &Params, json &Result);
};

}; // namespace lsp
#endif