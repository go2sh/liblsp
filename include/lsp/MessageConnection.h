#ifndef LSP_MESSAGECONNECTION_H
#define LSP_MESSAGECONNECTION_H

#include "lsp/LSProtocol.h"
#include "lsp/Logger.h"
#include "lsp/MessageReader.h"
#include "lsp/MessageWriter.h"

#include <condition_variable>
#include <functional>
#include <map>
#include <queue>
#include <thread>

#include <json.hpp>

using json = nlohmann::json;

namespace lsp {

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
  std::queue<JsonPtr> RequestQueue;
  /// Lookup map for request handlers
  std::map<std::string, RequestHandler> RequestHandlers;
  /// Lookup map for response data
  std::map<std::string, JsonPtr> ResponseMap;

  // Thread mutexes
  std::mutex RequestMutex;
  std::mutex ResponseMutex;
  std::mutex WriterMutex;

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

  void setLogger(Logger *NewLog) {
    delete Log;
    Log = NewLog;
  }

  /// Start listening for incomming messages
  void listen();

  /** Reply to request with an error
   *
   */
  template <typename T>
  void replyError(const json &Id, const ErrorResponse<T> &Response) {
    JsonPtr Reply = std::make_shared<json>();
    (*Reply)["jsonrpc"] = "2.0";
    (*Reply)["id"] = Id;
    (*Reply)["error"] = Response.dump();

    Writer->write(Reply);
  }

  /** Reply with an error
   *
   */
  template <typename T> void replyError(const ErrorResponse<T> &Response) {
    replyError(json(), Response);
  }

  /** Call a remote method
   */
  JsonPtr call(const std::string &Method, const json &Params);
  void request(const std::string &Method, const std::string &Id,
               const json &Data);
  void notify(const std::string &Method, const json &Data);

  void registerHandler(const std::string &Method, RequestHandler Handler) {
    RequestHandlers[Method] = Handler;
  }

  void processMessageQueue();

private:
  void errorHandler(const std::string &Message);
  void closeHandler();
  void messageHandler(JsonPtr Data);
};
} // namespace lsp
#endif