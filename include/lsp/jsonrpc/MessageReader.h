#ifndef LSP_MESSAGE_READER_H
#define LSP_MESSAGE_READER_H

#include <lsp/jsonrpc/Event.h>
#include <lsp/jsonrpc/Message.h>

#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

namespace lsp {
typedef Event::Emitter<const Error &>::CallbackFunction ErrorFunction;
typedef Event::Emitter<>::CallbackFunction CloseFunction;
typedef std::function<void(MessagePtr)> CallbackFunction;

enum class MessageReaderState { Disconnected, Connected, Closed };

class MessageReader {
protected:
  MessageReaderState State = MessageReaderState::Disconnected;

  // Callbacks for the MessageConnection
  /// Message callback
  CallbackFunction MessageHandler;
  /// Error callback
  /// Called when an error happend, while receiving data
  Event::Emitter<const Error &> ErrorEmitter;
  /// Close callback
  /// Called when the underlying connection went away
  Event::Emitter<> CloseEmitter;

public:
  /// Set the error handler function
  void onError(ErrorFunction Handler) { ErrorEmitter.on(Handler); }
  /// Set the close handler function
  void onClose(CloseFunction Handler) { CloseEmitter.on(Handler); }

  /** Start listening for new messages
   *
   * Start listening for new messages on the underlying connection. If
   * a message is successfuly received, the message callback will
   * be called. Depending on the underlying implementation this
   * function might block.
   */
  virtual void listen(CallbackFunction Callback) = 0;

protected:
  static std::size_t getLength(const std::string &Header);
};
} // namespace lsp
#endif