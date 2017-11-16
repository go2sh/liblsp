
#include "MessageConnection.h"

using namespace lsp;

void MessageConnection::errorHandler(const std::string &Message) {
  Log->logError(Message);
}

void MessageConnection::closeHandler() {}

static std::string getIdString(json &Message) {
  if (Message["id"].is_string())
    return Message["id"].get<std::string>();
  if (Message["id"].is_number_float())
    return std::to_string(Message["id"].get<float>());
  if (Message["id"].is_number_integer())
    return std::to_string(Message["id"].get<int>());
  if (Message["id"].is_number_float())
    return std::to_string(Message["id"].get<unsigned>());
  return std::string();
}

void MessageConnection::messageHandler(JsonPtr Data) {
  json &Message = *Data;
  if (!Message.is_object()) {
    Log->logError("JSON Message needs to be an object.");
  }
  if (!Message["jsonrpc"].is_string() ||
      !Message["jsonrpc"].get<std::string>().compare("2.0")) {
    Log->logError("Expected jsonrpc object.");
  }
  MessageQueue.push(Data);
};

void MessageConnection::run() {
  QueueThread = std::thread([this] { processMessageQueue(); });

  while (true) {
    Reader->read();
  }
}

void MessageConnection::processMessageQueue() {
  while (true) {
    if (MessageQueue.size() == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } else {
      JsonPtr Data = MessageQueue.front();
      json &Message = *Data;
      if (Message["method"].is_string()) {
        std::string &Method = Message["method"].get_ref<std::string &>();
        if (Message["id"].is_string() || Message["id"].is_number()) {
          onRequest(Method, getIdString(Message), Message);
        } else {
          // Notification
          onNotification(Method, Message);
        }
      } else {
        if (Message.find("result") != Message.end()) {
          // Response
          onResponse(getIdString(Message), Message);
        }
        if (Message.find("error") != Message.end()) {
          // Error
          onError(getIdString(Message), Message);
        }
      }
      MessageQueue.pop();
    }
  }
}