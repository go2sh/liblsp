#ifndef LSP_MESSAGE_H
#define LSP_MESSAGE_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace lsp {
class Message;
typedef std::unique_ptr<Message> MessagePtr;
class Message {
public:
  enum MessageType { Invalid, Request, Response, Notification };

protected:
  json Data;
  MessageType Type;

  Message(MessageType Type) : Type(Type) { Data["jsonrpc"] = "2.0"; }
  Message(MessageType Type, json &&Data) : Type(Type), Data(Data) {}

public:
  Message() : Type(Invalid) {}
  Message(json &&Data) : Type(Invalid), Data(Data) {}
  virtual ~Message() {}

  MessageType getType() const { return Type; }

  bool isInvalid() { return Type == Invalid; }
  bool isRequest() { return Type == Request; }
  bool isResponse() { return Type == Response; }
  bool isNotification() { return Type == Notification; }

  static MessagePtr parse(std::istream &Stream);
  friend std::ostream &operator<<(std::ostream &Stream, Message &Msg) {
    return Stream << Msg.Data;
  }
};

class RequestMessage : public Message {
public:
  RequestMessage(std::string &Method, std::string &Id)
      : Message(Message::MessageType::Request) {
    Data["id"] = Id;
    Data["method"] = Method;
  }
  RequestMessage(json &&Data)
      : Message(Message::MessageType::Request, std::move(Data)) {}

  std::string getId() {
    json &Id = Data.at("id");
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
  std::string getMethod() { return Data.at("method"); }

  json &getParams() { return Data.at("params"); }
};

class ResponseMessage : public Message {
public:
  ResponseMessage(std::string Id) : Message(Response) { Data["id"] = Id; }
  ResponseMessage(json &&Data)
      : Message(Message::MessageType::Response, std::move(Data)) {}

  bool isError() { return Data.find("error") != Data.end(); }
  std::string getId() {
    json &Id = Data.at("id");
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
  json &getResult() { return Data["result"]; }
  json &getError() { return Data["error"]; }

  static MessagePtr fromRequest(RequestMessage &Msg);
};

class NotificationMessage : public Message {
public:
  NotificationMessage(const std::string Method)
      : Message(Message::MessageType::Notification) {
    Data["method"] = Method;
  }
  NotificationMessage(json &&Data)
      : Message(Message::MessageType::Notification, std::move(Data)) {}

  std::string getMethod() const { return Data.at("method"); }
  json &getParams() { return Data["params"]; }
};

enum ErrorCode {
  ParseError = -32700,
  InvalidRequest = -32600,
  MethodNotFound = -32601,
  InvalidParams = -32602,
  InternalError = -32603
};

class Error : public std::exception {
  json Data;

public:
  Error() { Data["code"] = 0; }

  Error(int Code, const std::string &Message) {
    Data["code"] = Code;
    Data["message"] = Message;
  }

  virtual std::string what() { return Data["message"]; }
  std::string message() { return Data.at("message"); }
  int code() { return Data.at("code"); }
};

class MessageType {
  std::string Method;

public:
  MessageType(std::string &Method) : Method(Method) {}

  std::string method() const { return Method; }
};

template <typename ParamType, typename ReturnType>
class RequestType : public MessageType {
public:
  RequestType(std::string Method) : MessageType(Method) {}

  std::function<void(const json &, json &)>
  getRequestFunction(std::function<ReturnType(const ParamType &)> Func) {
    return [Func](const json &JsonParams, json &JsonResult) {
      ParamType Params = JsonParams;
      ReturnType Result = Func(Params);
      JsonResult = Result;
    };
  }
};

template <typename ParamType> class NotificationType : public MessageType {
public:
  NotificationType(std::string Method) : MessageType(Method) {}

  std::function<void(const json &)>
  getNotificationFunc(std::function<void(const ParamType &)> Func) {
    return [Func](const json &JsonParams) {
      ParamType Params = JsonParams;
      Func(Params);
    };
  }
};

} // namespace lsp
#endif // !LSP_MESSAGE_H