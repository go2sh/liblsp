#ifndef LSP_LANGUAGECLIENT_H
#define LSP_LANGUAGECLIENT_H
#include <lsp/protocol/LSProtocol.h>
#include <lsp/jsonrpc/MessageConnection.h>

namespace lsp {
class LanguageClient {

public:
  virtual void showMessage(const LogMessageParams &Params) = 0;
  virtual void logMessage(const LogMessageParams &Params) = 0;
  virtual void publishDiagnostics(const PublishDiagnosticsParams &Param) = 0;
};

class ProxyLanguageClient : public LanguageClient {
  MessageConnection &Connection;

public:
  ProxyLanguageClient(MessageConnection &Connection) : Connection(Connection) {}

  virtual void showMessage(const LogMessageParams &Params) {
    json Data = Params;
    Connection.sendNotification(ShowMessage, Params);
  }

  virtual void logMessage(const LogMessageParams &Params) {
    json Data = Params;
    //Connection.notify("window/logMessage", Data);
  }

  virtual void publishDiagnostics(const PublishDiagnosticsParams &Params) {
      json Data = Params;
      //Connection.notify("textDocument/publishDiagnostics",Data);
  }
};
}; // namespace lsp
#endif
