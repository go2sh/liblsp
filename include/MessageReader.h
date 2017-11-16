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

const char *LENGTH_HEADER = "Content-Length: ";

typedef std::vector<std::pair<std::string, std::string>> HeaderList;
class Message {
  asio::streambuf Buffer;
  json Message;
  HeaderList Headers;

private:
  std::size_t Length;

public:
  asio::streambuf &getBuffer() { return Buffer; }
  std::size_t getLength() { return Length; }
  void setLength(std::size_t Length) { Length = Length; }

  HeaderList &tryParseHeader();
  json &tryParseBody();
};

typedef std::function<void(const std::string &)> ErrorFunction;
typedef std::function<void(void)> CloseFunction;
typedef std::function<void(json &)> CallbackFunction;

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

  void listen(CallbackFunction Handler) {
    callback = Handler;
    read();
  }

private:
  virtual void read() = 0;
};

class SocketMessageReader : public MessageReader {
  tcp::socket &Socket;

public:
  SocketMessageReader(tcp::socket &Socket) : Socket(Socket) {}

private:
  virtual void read();
};

}; // namespace lsp
#endif