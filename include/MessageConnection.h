#ifndef LSP_MESSAGECONNECTION_H
#define LSP_MESSAGECONNECTION_H
#include <condition_variable>
#include <functional>
#include <map>
#include <queue>
#include <thread>

#include <json.hpp>

#include "Logger.h"
#include "LSProtocol.h"
#include "MessageReader.h"
#include "MessageWriter.h"

using json = nlohmann::json;

namespace lsp {

typedef std::function<void(const json &,json &)>
    RequestHandler;


class MessageConnection {
  MessageReader *Reader;
  MessageWriter *Writer;
  Logger *Log;

  std::queue<JsonPtr> RequestQueue;
  std::map<std::string,JsonPtr> ResponseMap;

  std::map<std::string,RequestHandler> RequestHandlers;

  std::thread QueueThread;
  std::mutex RequestMutex;
  std::mutex ResponseMutex;
  std::mutex WriterMutex;

  std::condition_variable RequestCV;
  std::condition_variable ResponseCV;

  uint32_t IdCounter = 0;

public:
  MessageConnection(MessageReader *Reader, MessageWriter *Writer, Logger *Log)
      : Reader(Reader), Writer(Writer), Log(Log) {
    Reader->setErrorHandler(std::bind(&MessageConnection::errorHandler, this,
                                      std::placeholders::_1));
    Reader->setCloseHandler(std::bind(&MessageConnection::closeHandler, this));

    Reader->listen(std::bind(&MessageConnection::messageHandler, this,
                             std::placeholders::_1));
  };

  void run();
  template<typename T>
  void replyError(const json & Id, const ErrorResponse<T> & Response);
  template<typename T>
  void replyError(const ErrorResponse<T> & Response);

  JsonPtr call(const std::string & Method, const json & Params);
  void request(const std::string & Method, const std::string & Id, const json & Data);
  void notify(const std::string & Method, const json & Data);

  void registerHandler(const std::string & Method, RequestHandler Handler) {
    RequestHandlers[Method] = Handler;
  }

  void processMessageQueue();

private:
  void errorHandler(const std::string &Message);
  void closeHandler();
  void messageHandler(JsonPtr Data);
};
}; // namespace lsp
#endif