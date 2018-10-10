#include <asio.hpp>
#include <lsp/lsp.h>
#include <lsp/protocol/LSProtocol.h>

#include <cstdlib>
#include <iostream>
#include <thread>

using asio::ip::tcp;

lsp::InitializeResult asd(const lsp::InitializeParams<std::string> &Params) {
  lsp::InitializeResult res;
  res.Capabilities.DocumentFormattingProvider = true;
  res.Capabilities.TextDocumentSync.OpenClose = true;
  return res;
}
std::vector<lsp::TextEdit> format(const lsp::DocumentFormattingParams &Params) {
  std::vector<lsp::TextEdit> Edits;
  lsp::TextEdit Edit;
  Edit.NewText = "Hello";
  Edit.Range = {{1,1}, {1,1}};
  Edits.push_back(std::move(Edit));
  return Edits;
}

int main(int argc, char *argv[]) {
  asio::io_service service;
  lsp::MessageConnection *Con = lsp::createAsioTCPConnection(service, "localhost","19191");
  std::function<lsp::InitializeResult(const lsp::InitializeParams<std::string>&)> Func(&asd);
  Con->registerRequestHandler(lsp::InitializeRequest, Func);
  Con->registerRequestHandler(lsp::TextDocumentFormatting, std::function(&format));
  Con->listen();

  try {
      std::thread asd([Con] {
    while (true)
    Con->processMessageQueue();
  });
     service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}