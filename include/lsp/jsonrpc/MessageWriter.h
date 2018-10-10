#ifndef LSP_MESSAGEWRITER_H
#define LSP_MESSAGEWRITER_H

#include <lsp/jsonrpc/Message.h>

namespace lsp {

/// Interface for writing sending messages
class MessageWriter {

public:
  /// Send a message
  virtual void write(MessagePtr Data) = 0;
};
}; // namespace lsp
#endif