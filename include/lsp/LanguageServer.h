#ifndef LSP_LANGUAGESERVER_H
#define LSP_LANGUAGESERVER_H

#include "lsp/LSProtocol.h"
#include "lsp/MessageConnection.h"

#include <functional>
#include <type_traits>

namespace lsp {
class LanguageServer {

public:
  void connect(MessageConnection &Connection);

  /*
   * General functions
   */
  virtual InitializeResult
  initialize(const InitializeParams<EmptyInitializationOptions> &Params);
  virtual void initialized(const InitializedParams &Params);
  virtual void shutdown();
  virtual void exit();
  //virtual void cancleRequest(const std::string &Id);
  
  virtual Hover hover(const TextDocumentPositionParams &Params);
  virtual std::vector<CompletionItem<EmptyCompletionData>>
  completion(const CompletionParams &Params);
  virtual void
  textDocumentDidChange(const TextDocumentDidChangeParams &Params);
};

}; // namespace lsp
#endif