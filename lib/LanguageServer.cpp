#include "LanguageServer.h"
#include "LSProtocol.h"

using namespace lsp;

void LanguageServer::handleInitialize(const json &Params, json &Result) {
  InitializeParams<EmptyInitializationOptions> InitParams = Params;
  InitializeResult InitResult = initialize(InitParams);
  Result = InitResult;
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