#include <lsp/jsonrpc/Message.h>

using namespace lsp;

bool checkRequest(const json &Data) {
  return Data.find("method") != Data.end() && Data.find("id") != Data.end();
}

bool checkResponse(const json &Data) {
  return (Data.find("result") != Data.end() ||
          Data.find("error") != Data.end()) &&
         Data.find("id") != Data.end();
}

bool checkNotification(const json &Data) {
  return Data.find("method") != Data.end() && Data.find("id") == Data.end();
}

MessagePtr Message::parse(std::istream &Stream) {
  MessagePtr Msg;
  json Data;

  try {
    Stream >> Data;
  } catch (json::parse_error& e) {
    throw Error(ErrorCode::ParseError, std::string(e.what()));
  }

  if (checkRequest(Data)) {
    Msg = std::make_unique<RequestMessage>(std::move(Data));
  } else if (checkNotification(Data)) {
    Msg = std::make_unique<NotificationMessage>(std::move(Data));
  } else if (checkResponse(Data)) {
    Msg = std::make_unique<ResponseMessage>(std::move(Data));
  } else {
    Msg = std::make_unique<Message>(std::move(Data));
  }

  return Msg;
}

MessagePtr ResponseMessage::fromRequest(RequestMessage &Msg) {
  return std::make_unique<ResponseMessage>(Msg.getId());
}

MessagePtr createNotification(const std::string &Method) {
  std::unique_ptr<NotificationMessage> Msg = std::make_unique<NotificationMessage>(Method);

  return Msg;
}