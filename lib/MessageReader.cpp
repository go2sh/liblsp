#include <functional>
#include <string>

#include <asio.hpp>
#include <json.hpp>

#include "MessageReader.h"

using json = nlohmann::json;
using namespace lsp;

HeaderList &Message::tryParseHeader() {
  std::string Header(asio::buffers_begin(Buffer.data()),
                     asio::buffers_end(Buffer.data()));
  Headers.clear();

  std::size_t ColonPos = 0, LineEndPos = 0, LineStartPos = 0;
  while (true) {
    ColonPos = Header.find(':', LineStartPos);
    LineEndPos = Header.find("\r\n", ColonPos);
    if (ColonPos != std::string::npos || LineEndPos != std::string::npos) {
      break;
    }
    Headers.push_back(
        std::make_pair(Header.substr(LineStartPos, ColonPos - LineStartPos),
                       Header.substr(ColonPos + 1, LineEndPos - ColonPos - 1)));
    LineStartPos = LineEndPos + 2;
  }
  return Headers;
}

json &Message::tryParseBody() {
    std::istream Stream(&Buffer);
    try {
        Message = json::parse(Stream);
    } catch (json::parse_error) {
        Message = json();
    }

    return Message;
}

bool MessageReader::checkMessageHeader() {
  for (auto const &Element : Msg.tryParseHeader()) {
    if (Element.first.compare(LENGTH_HEADER) == 0) {
      Msg.setLength(stoul(Element.second));
      return true;
    }
  }
  return false;
}

void SocketMessageReader::read() {
  asio::streambuf Buf;
  asio::read_until(Socket, Msg.getBuffer(), std::string("\r\n\r\n"));
  
  if (!checkMessageHeader()) {
      error("No valid Content-Length found in header data.");
      return;
  }
  
  asio::read(Socket, Msg.getBuffer());

  json Data = Msg.tryParseBody();
  if (Data.is_null()) {
      error("Failed to parse json data.");
      return;
  }
  callback(Data);
}