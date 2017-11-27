#include "LanguageServer.h"
#include "LanguageClient.h"
#include "MessageConnection.h"
#include "ServerConnection.h"

class TestServer : public lsp::LanguageServer {
  lsp::LanguageClient *Client;
public:
  TestServer(lsp::LanguageClient *Client = nullptr) : Client(Client) {
  }
  virtual lsp::InitializeResult
  initialize(const lsp::InitializeParams<lsp::EmptyInitializationOptions> & Params) {
    lsp::InitializeResult Result;
    Result.Capabilities.HoverProvider = true;
    Result.Capabilities.CompletionProvider = {true, {".","'"}};

    return Result;
  }

  virtual lsp::Hover hover(const lsp::TextDocumentPositionParams & Params) {
    lsp::Hover H;
    H.Contents.Language = "vhdl";
    H.Contents.Value = "Works";
    lsp::PublishDiagnosticsParams Diag;
    Diag.Uri = "file:///home/chris/Coding/hdl/test.vhd";
    lsp::Diagnostic D;
    D.Severity = lsp::DiagnosticSeverity::Error;
    D.Message = "Test message";
    D.Source = "vhdl-ls";
    D.Range.Start = {0,0};
    D.Range.End = {0, 1};
    Diag.Diagnostics.push_back(D);
    Client->publishDiagnostics(Diag);
    lsp::LogMessageParams L;
    L.type = lsp::MessageType::Error;
    L.message = "asd fff";
    Client->showMessage(L);
    return H;
  }

  virtual std::vector<lsp::CompletionItem<lsp::EmptyCompletionData>> completion(const lsp::CompletionParams &Params) {
    lsp::CompletionItem<lsp::EmptyCompletionData> I;
    std::vector<lsp::CompletionItem<lsp::EmptyCompletionData>> Result;
    I.Label = "hase hase";
    Result.push_back(I);
    return Result;
  }
};

int main(int argc, char **argv) {
  lsp::StdinMessageReader Reader;
  lsp::StdoutMessageWriter Writer;
  lsp::StdIoLogger Log;
  lsp::MessageConnection Connection(&Reader, &Writer, &Log);
  lsp::ProxyLanguageClient Client(Connection);
  TestServer Server(&Client);
  Server.connect(Connection);

  std::thread ConnectionThread =
      std::thread([&Connection] { Connection.run(); });
  while (true) {
    Connection.processMessageQueue();
  }

  ConnectionThread.join();
}