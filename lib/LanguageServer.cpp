#include "LanguageServer.h"
#include "LSProtocol.h"

using namespace lsp;

void LanguageServer::handleInitialize(const json &Params, json &Result) {
  InitializeParams<EmptyInitializationOptions> InitParams = Params;
  InitializeResult InitResult = initialize(InitParams);
  Result = InitResult;
}

void LanguageServer::handleShutdown(const json &Params, json &Result) {
  shutdown();
}

void LanguageServer::handleExit(const json &Params, json &Result) {
  exit();
}

void LanguageServer::handleHover(const json &Params, json &Result) {
  TextDocumentPositionParams Position = Params;
  Hover HoverResult = hover(Position);
  Result = HoverResult;
}

void LanguageServer::handleCompletion(const json &Params, json &Result) {
  CompletionParams CP = Params;
  std::vector<CompletionItem<EmptyCompletionData>> CL = completion(Params);
  Result = CL;
}

void LanguageServer::handleTextDocumentDidChange(const json &Params, json &Result) {
  TextDocumentDidChangeParams CP = Params;
  textDocumentDidChange(CP);
}