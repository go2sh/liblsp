#ifndef LSP_MESSAGE_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace lsp {
enum MessageType { Request, Response, Notifiaction, Error };

class Message {
protected:
  json Data;
  MessageType Type;

public:
  virtual ~Message() {}
  MessageType getType() const { return Type; }

  void dump(std::ostream &Stream);

  bool isRequest() { return Type == Request; }
  bool isResponse() { return Type == Response; }
  bool isNotification() { return Type == Notifiaction; }
  bool isError() { return Type == Error; }
};

class RequestMessage : public Message {
  std::string Id;
  std::string Method;

public:
  std::string getId() { return Id; }
  std::string getMethod() { return Method; }

  json &getParams() { return Data.at("params"); }
};


} // namespace lsp
#endif // !LSP_MESSAGE_H