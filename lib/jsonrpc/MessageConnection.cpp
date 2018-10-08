#include <lsp/jsonrpc/MessageConnection.h>

using namespace lsp;

void MessageConnection::errorHandler(const Error &Message) {}

void MessageConnection::closeHandler() {}

void MessageConnection::messageHandler(MessagePtr Msg) {
  switch (Msg->getType()) {
  case Message::MessageType::Invalid:
    return;
  case Message::MessageType::Notification:
  case Message::MessageType::Request: {
    std::unique_lock<std::mutex> Lock(RequestMutex);
    RequestQueue.push(std::move(Msg));
    RequestCV.notify_one();
  }
  case Message::MessageType::Response: {
    std::unique_lock<std::mutex> Lock(ResponseMutex);
    ResponseMap[static_cast<ResponseMessage*>(Msg.get())->getId()] = std::move(Msg);
    ResponseCV.notify_all();
  }
  }
}

void MessageConnection::listen() {
  Reader->listen(std::bind(&MessageConnection::messageHandler, this,
                           std::placeholders::_1));
}

#if 0
void MessageConnection::notify(const std::string &Method, const json &Data) {
  JsonPtr Request = std::make_shared<json>();

  (*Request)["jsonrpc"] = "2.0";
  (*Request)["method"] = Method;
  (*Request)["params"] = Data;

  Writer->write(Request);
}

JsonPtr MessageConnection::call(const std::string &Method, const json &Data) {
  std::stringstream IdStream;
  IdStream << Method << "-" << IdCounter++;

  std::string Id = IdStream.str();
  request(Method, Id, Data);

  std::unique_lock<std::mutex> Lock(ResponseMutex);
  ResponseCV.wait(Lock, [this, &Id]() {
    return ResponseMap.find(Id) != ResponseMap.end();
  });

  auto Itr = ResponseMap.find(Id);
  JsonPtr Ptr = Itr->second;
  json &Response = *Ptr;

  ResponseMap.erase(Itr);
  if (Response.find("error") != Response.end()) {
    // Maybe throw
  }
  if (Response.find("result") == Response.end()) {
    // Maybe throw
  }
  return Ptr;
}

void MessageConnection::request(const std::string &Method,
                                const std::string &Id, const json &Data) {
  JsonPtr Request = std::make_shared<json>();

  (*Request)["jsonrpc"] = "2.0";
  (*Request)["method"] = Method;
  (*Request)["id"] = Id;
  (*Request)["params"] = Data;

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
  auto Handler = RequestHandlers.find(Method);
  JsonPtr Response = std::make_shared<json>();

  if (Handler != RequestHandlers.end()) {
    if (Id != Message.end()) {
      (*Response)["jsonrpc"] = "2.0";
      (*Response)["id"] = *Id;

      try {
        (Handler->second)(Message["params"], (*Response)["result"]);
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
      try {
        (Handler->second)(Message["params"], (*Response)["result"]);
      } catch (...) {
      }
    }
  } else {
    if (Id != Message.end()) {
      replyError(*Id, ErrorResponse<EmptyResponse>(ErrorCode::MethodNotFound,
                                                   "Method not found"));
    } else {
      Log->error("Notification request to unimplemented method:" + Method);
    }
  }
  RequestQueue.pop();
}
#endif