#include <sstream>

#include "LSProtocol.h"
#include "MessageConnection.h"

using namespace lsp;

void MessageConnection::errorHandler(const std::string &Message) {
  Log->logError(Message);
}

void MessageConnection::closeHandler() {}

static std::string getId(json &Id) {
  if (Id.is_string())
    return Id.get<std::string>();
  if (Id.is_number_float())
    return std::to_string(Id.get<float>());
  if (Id.is_number_integer())
    return std::to_string(Id.get<int>());
  if (Id.is_number_float())
    return std::to_string(Id.get<unsigned>());
  return std::string("");
}

void MessageConnection::messageHandler(JsonPtr Data) {
  json &Message = *Data;

  // Check for parser error
  if (!Message.is_object()) {
    replyError(ErrorResponse<EmptyResponse>(
        ErrorCode::ParseError, "Failed to parse message string into json."));
    return;
  }

  // Check for valid json-rpc message with version 2.0
  auto Version = Message.find("jsonrpc");
  std::string s = (*Version).get<std::string>();

  if (Version == Message.end() || !(*Version).is_string() ||
      (*Version).get<std::string>().compare("2.0")) {
    replyError(ErrorResponse<EmptyResponse>(
        ErrorCode::InvalidRequest, "Expected jsonrpc object version 2.0."));
    return;
  }

  auto Method = Message.find("method");
  auto Id = Message.find("id");

  // Check type for method, if it exists
  if (Method != Message.end() && !Method->is_string()) {
    replyError(ErrorResponse<EmptyResponse>(ErrorCode::InvalidRequest,
                                            "Method type is invalid."));
  }

  // Check type for id, if it exists
  if (Id != Message.end() &&
      !((Id->is_string() || Id->is_number() || Id->is_null()))) {
    replyError(ErrorResponse<EmptyResponse>(ErrorCode::InvalidRequest,
                                            "Id type is invalid."));
  }

  if (Method != Message.end()) {
    // Handle msg as request or notification
    std::unique_lock<std::mutex> Lock(RequestMutex);
    RequestQueue.push(Data);
    RequestCV.notify_one();
  } else if (Id != Message.end()) {
    // Handle msg as response
    std::unique_lock<std::mutex> Lock(ResponseMutex);
    ResponseMap[getId(*Id)] = Data;
    ResponseCV.notify_all();
  } else {
    replyError(ErrorResponse<EmptyResponse>(ErrorCode::InvalidRequest,
                                            "Method or id is required."));
  }
};

void MessageConnection::run() {
  while (true) {
    Reader->read();
  }
}

template <typename T>
void MessageConnection::replyError(const json &Id,
                                   const ErrorResponse<T> &Response) {
  json Reply;
  Reply["jsonrpc"] = "2.0";
  Reply["id"] = Id;
  Reply["error"] = Response.dump();

  Writer->write(Reply);
}

template <typename T>
void MessageConnection::replyError(const ErrorResponse<T> &Response) {
  replyError(json(), Response);
}

void MessageConnection::notify(const std::string &Method, const json &Data) {
  json Request;

  Request["jsonrpc"] = "2.0";
  Request["method"] = Method;
  Request["params"] = Data;

  Writer->write(Request);
}

void MessageConnection::call(const std::string &Method, const json &Data) {
  stringstream IdStream;
  IdStream << Method << "-" << IdCounter;

  std::string Id = IdStream.str();
  request(Method, Id, Data);

  std::unique_lock<std::mutex> Lock(ResponseMutex);
  ResponseCV.wait(
      Lock, [this]() { return ResponseMap.find(Id) != ResponseMap.end(); });
}

void MessageConnection::request(const std::string &Method,
                                const std::string &Id, const json &Data) {
  json Request;

  Request["jsonrpc"] = "2.0";
  Request["method"] = Method;
  Request["id"] = Id;
  Request["params"] = Data;

  Writer->write(Request);
}

void MessageConnection::processMessageQueue() {
  std::unique_lock<std::mutex> Lock(RequestMutex);

  // Wait for the queue to be filled
  if (RequestQueue.size() == 0) {
    RequestCV.wait(Lock, [this]() { return RequestQueue.size() != 0; });
  }

  JsonPtr Data = RequestQueue.front();
  json &Message = *Data;
  std::string Method = Message.find("method")->get<std::string>();
  auto Id = Message.find("id");
  json Response;

  if (Id != Message.end()) {
    auto Handler = RequestHandlers.find(Method);
    if (Handler != RequestHandlers.end()) {
      Response["jsonrpc"] = "2.0";
      Response["id"] = *Id;

      try {
        (Handler->second)(Message, Response["result"]);
      } catch (const json::out_of_range &E) {
        replyError(*Id, ErrorResponse<EmptyResponse>(ErrorCode::InvalidParams,
                                                     E.what()));
      } catch (...) {
        replyError(*Id,
                   ErrorResponse<EmptyResponse>(ErrorCode::InternalError,
                                                "Unexpected error happend"));
      }
      Writer->write(Response);
    } else {
      replyError(*Id, ErrorResponse<EmptyResponse>(ErrorCode::MethodNotFound,
                                                   "Method not found"));
    }
  } else {
    auto Handler = NotificationHandlers.find(Method);
    if (Handler != NotificationHandlers.end()) {
      try {
        (Handler->second)(Message, Response["result"]);
      } catch (...) {
      }
    } else {
      Logger->logError("Notification request to unimplemented method:" + Method);
    }
  }

  RequestQueue.pop();
}