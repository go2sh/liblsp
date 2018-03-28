#ifndef LSP_MESSAGEHANDLER_H
#define LSP_MESSAGEHANDLER_H

#include "lsp/Event.h"

#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

namespace lsp {

typedef std::shared_ptr<nlohmann::json> JsonPtr;
typedef Event::Emitter<const std::string&>::CallbackFunction ErrorFunction;
typedef Event::Emitter<>::CallbackFunction  CloseFunction;
typedef std::function<void(const JsonPtr)> CallbackFunction;

class Message {
  std::size_t Length;
  JsonPtr Data;

public:
  std::size_t getLength() { return Length; }
  JsonPtr getData() { return Data; }

  bool parseHeader(const std::string &Header);
  bool parseBody(const std::string &Data);
};

enum class MessageReaderState { Disconnected, Connected, Closed };

class MessageReader {
protected:
  MessageReaderState State = MessageReaderState::Disconnected;
  Message CurrentMessage;

  // Callbacks for the MessageConnection
  /// Message callback
  CallbackFunction MessageHandler;
  /// Error callback
  /// Called when an error happend, while receiving data
  Event::Emitter<const std::string&> ErrorEmitter;
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
};
} // namespace lsp
#endif