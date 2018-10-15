#ifndef LSP_REMOTE_H
#define LSP_REMOTE_H

#include <lsp/jsonrpc/MessageConnection.h>
#include <lsp/protocol/LSProtocol.h>

namespace lsp {
class Remote {
public:
  void attach(MessageConnectionPtr MsgConnection) {
    Connection = MsgConnection;
  }
  
  virtual void initialize(const ClientCapabilities &CC) = 0;

protected:
  MessageConnectionPtr Connection;

};

class Remote
} // namespace lsp

#endif // DEBUG