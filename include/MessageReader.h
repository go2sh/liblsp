#ifndef LSP_MESSAGEREADER_H
#define LSP_MESSAGEREADER_H

#include <functional>
#include <string>
#include <vector>

#include <asio.hpp>
#include <json.hpp>

using asio::ip::tcp;
using json = nlohmann::json;

namespace lsp {

typedef std::shared_ptr<json> JsonPtr;
typedef std::vector<std::pair<std::string, std::string>> HeaderList;
class Message {
  std::vector<char> Buffer;
  JsonPtr Data;
  HeaderList Headers;

private:
  std::size_t Length;

public:
  static const char *LENGTH_HEADER;

  Message() : Buffer() { Buffer.reserve(1024); }

  std::vector<char> &getBuffer() { return Buffer; }
  std::size_t getLength() { return Length; }
  HeaderList &getHeaders() { return Headers; }
  JsonPtr getData() { return Data; }
  void setLength(std::size_t Len) { Length = Len; }
  void appendBuffer(const char &Byte) { Buffer.push_back(Byte); }

  bool tryParseHeader();
  bool tryParseBody();
};

typedef std::function<void(const std::string &)> ErrorFunction;
typedef std::function<void(void)> CloseFunction;
typedef std::function<void(JsonPtr)> CallbackFunction;

class MessageReader {

protected:
  Message Msg;
  CallbackFunction callback;
  CloseFunction close;
  ErrorFunction error;

  bool checkMessageHeader();

public:
  void setErrorHandler(ErrorFunction Handler) { error = Handler; }
  void setCloseHandler(CloseFunction Handler) { close = Handler; }

  void listen(CallbackFunction Handler) { callback = Handler; }

  virtual void read() = 0;
};

class SocketMessageReader : public MessageReader {
  tcp::socket &Socket;

public:
  SocketMessageReader(tcp::socket &Socket) : Socket(Socket) {}

  virtual void read();
};

class StdinMessageReader : public MessageReader {
public:
  StdinMessageReader() {}

  virtual void read();
};
}; // namespace lsp
#endif