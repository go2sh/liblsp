#ifndef LSP_MESSAGECONNECTION_H
#define LSP_MESSAGECONNECTION_H

#include <lsp/Logger.h>
#include <lsp/jsonrpc/MessageReader.h>
#include <lsp/jsonrpc/MessageWriter.h>
#include <lsp/protocol/LSProtocol.h>

#include <condition_variable>
#include <functional>
#include <map>
#include <queue>
#include <thread>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace lsp {

typedef std::function<void(const json &)> NotificationHandler;
typedef std::function<void(const json &, json &)> RequestHandler;

/// Message connection for passing messages between client and server
/// The MessageConnection class is thread save for all operations.
class MessageConnection {
  /// Message reader
  MessageReader *Reader;
  /// Message writer
  MessageWriter *Writer;
  /// Logger
  Logger *Log;

  /// Queue for incomming JSON-RPC requests
  std::queue<MessagePtr> RequestQueue;
  /// Lookup map for request handlers
  std::map<std::string, RequestHandler> RequestHandlers;
  /// Lookup map for notification handlers
  std::map<std::string, NotificationHandler> NotificationHandlers;
  /// Lookup map for response data
  std::map<std::string, MessagePtr> ResponseMap;

  // Thread mutexes
  std::mutex RequestMutex;
  std::mutex ResponseMutex;

  // Thread condition variables
  std::condition_variable RequestCV;
  std::condition_variable ResponseCV;

  /** MessageID counter
   * The counter may overflow and overwrite outstanding requests on the other
   * connections side.
   */
  unsigned long IdCounter = 0;

public:
  MessageConnection(MessageReader *Reader, MessageWriter *Writer)
      : Reader(Reader), Writer(Writer), Log(new NullLogger()) {
    Reader->onError(std::bind(&MessageConnection::errorHandler, this,
                              std::placeholders::_1));
    Reader->onClose(std::bind(&MessageConnection::closeHandler, this));
  };
  MessageConnection(MessageReader *Reader, MessageWriter *Writer, Logger *Log)
      : Reader(Reader), Writer(Writer), Log(Log) {
    Reader->onError(std::bind(&MessageConnection::errorHandler, this,
                              std::placeholders::_1));
    Reader->onClose(std::bind(&MessageConnection::closeHandler, this));
  };

  /// Start listening for incomming messages
  void listen();

  template <typename T1, typename T2>
  void registerRequestHandler(RequestType<T1, T2> &Type,
                              std::function<T2(const T1 &)> Func) {
    RequestHandlers[Type.method()] = Type.getRequestFunction(Func);
  }

  template <typename ParamType>
  void
  registerNotificationHandler(NotificationType<ParamType> &Type,
                              std::function<void(const ParamType &)> Func) {
    NotificationHandlers[Type.method()] = Type.getNotificationFunc(Func);
  }

  template <typename ParamType>
  void sendNotification(NotificationType<ParamType> Type, const ParamType &Params) {
    std::unique_ptr<NotificationMessage> Msg = std::make_unique<NotificationMessage>(Type.method());
    json &JsonParams = Msg->getParams();
    JsonParams = Params;
    Writer->write(std::move(Msg));
  }

  void processMessageQueue();

private:
  void errorHandler(const Error &Err);
  void closeHandler();
  void messageHandler(MessagePtr Data);

  void handleRequest(RequestMessage &Msg);
  void handleNotification(NotificationMessage &Msg);
  void handleResponse(ResponseMessage &Msg);
  void handleInvalid(Message &Msg);
};

std::shared_ptr<MessageConnection> MesssageConnectionPtr;
} // namespace lsp
#endif