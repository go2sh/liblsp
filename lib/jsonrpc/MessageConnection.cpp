#include <lsp/jsonrpc/MessageConnection.h>

using namespace lsp;

void MessageConnection::errorHandler(const Error &Message) {}

void MessageConnection::closeHandler() {}

void MessageConnection::messageHandler(MessagePtr Msg) {
  switch (Msg->getType()) {
  case Message::MessageType::Invalid:
    break;
  case Message::MessageType::Notification:
  case Message::MessageType::Request: {
    std::unique_lock<std::mutex> Lock(RequestMutex);
    RequestQueue.push(std::move(Msg));
    RequestCV.notify_one();
    break;
  }
  case Message::MessageType::Response: {
    std::unique_lock<std::mutex> Lock(ResponseMutex);
    ResponseMap[static_cast<ResponseMessage*>(Msg.get())->getId()] = std::move(Msg);
    ResponseCV.notify_all();
    break;
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
#endif

void MessageConnection::processMessageQueue() {
  std::unique_lock<std::mutex> Lock(RequestMutex);

  // Wait for the queue to be filled
  if (RequestQueue.size() == 0) {
    RequestCV.wait(Lock, [this]() { return RequestQueue.size() != 0; });
  }

  MessagePtr Msg = std::move(RequestQueue.front());
  RequestQueue.pop();
  if (Msg->isRequest()) {
    RequestMessage *Req = static_cast<RequestMessage*>(Msg.get());
    MessagePtr RespMsg = ResponseMessage::fromRequest(*Req);
    ResponseMessage *Resp = static_cast<ResponseMessage*>(RespMsg.get());

    auto Handler = RequestHandlers.find(Req->getMethod());
    if (Handler != RequestHandlers.end()) {
      Handler->second(Req->getParams(), Resp->getResult());
    }

    Writer->write(std::move(RespMsg));
  }
}
