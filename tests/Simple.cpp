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

int main(int argc, char *argv[]) {
  asio::io_service service;
  lsp::MessageConnection *Con = lsp::createAsioTCPConnection(service, "localhost","19191");
  std::function<lsp::InitializeResult(const lsp::InitializeParams<std::string>&)> Func(&asd);
  Con->registerRequestHandler(lsp::InitializeRequest, Func);
  Con->listen();

  std::thread asd([Con] {
    while (true)
    Con->processMessageQueue();
  });
  service.run();
  return 0;
}