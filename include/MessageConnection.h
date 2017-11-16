#ifndef LSP_MESSAGECONNECTION_H
#define LSP_MESSAGECONNECTION_H
#include <functional>

#include <json.hpp>

#include "Logger.h"
#include "MessageReader.h"
#include "MessageWriter.h"

using json = nlohmann::json;

namespace lsp {

  typedef std::function<void(std::string&,std::string&, json&)> RequestHandler;
  typedef std::function<void(std::string&, json&)> NotificationHandler;
  typedef std::function<void(std::string&, json&)> ResponseHandler;
  typedef std::function<void(std::string&, json&)> ErrorHandler;

class MessageConnection {
  MessageReader *Reader;
  MessageWriter *Writer;
  Logger *Log;

  RequestHandler onRequest;
  NotificationHandler onNotification;
  ResponseHandler onResponse;
  ErrorHandler onError;

public:
  MessageConnection(MessageReader *Reader, MessageWriter *Writer,
                    Logger *Log) : Reader(Reader), Writer(Writer), Log(Log) {
    Reader->setErrorHandler(
        std::bind(&MessageConnection::errorHandler, this, std::placeholders::_1));
    Reader->setCloseHandler(std::bind(&MessageConnection::closeHandler, this));

    Reader->listen(std::bind(&MessageConnection::messageHandler,this,std::placeholders::_1));
  };

  void setRequestHandler(RequestHandler Handler) {onRequest = Handler;}
  void setNotificationHandler(NotificationHandler Handler) {onNotification = Handler;}
  void setResponseHandler(ResponseHandler Handler) {onResponse = Handler;}
  void setErrorHandler(ErrorHandler Handler) {onError = Handler;}

private:
  void errorHandler(const std::string &Message);
  void closeHandler();
  void messageHandler(json &Message);
};
}; // namespace lsp
#endif