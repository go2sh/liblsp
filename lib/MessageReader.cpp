#include <functional>
#include <iostream>
#include <istream>
#include <iterator>
#include <string>

#include "MessageReader.h"

using namespace lsp;

const char *Message::LENGTH_HEADER = "Content-Length";

bool Message::tryParseHeader() {
  std::size_t HeaderEnd = 0;
  std::size_t ColonPos = 0, LineEndPos = 0, LineStartPos = 0;

  Headers.clear();

  for (std::size_t i = 0; i < Buffer.size(); i++) {
    if (Buffer[i] == ':') {
      ColonPos = i;
    }
    if (Buffer[i] == '\n') {
      if (ColonPos != 0) {
        LineEndPos = i;
        Headers.push_back(std::make_pair(
            std::string(&Buffer[LineStartPos], ColonPos - LineStartPos),
            std::string(&Buffer[ColonPos + 1], LineEndPos - ColonPos - 1)));
        LineStartPos = i + 1;
        ColonPos = 0;
      } else {
        LineStartPos = i + 1;
      }
    }
    if ((i > 0 && Buffer[i - 1] == '\n' && Buffer[i] == '\n') ||
        (i > 2 && Buffer[i - 3] == '\r' && Buffer[i - 2] == '\n' &&
         Buffer[i - 1] == '\r' && Buffer[i] == '\n')) {
      HeaderEnd = i;
    }
  }

  if (HeaderEnd == 0) {
    return false;
  }

  if (Buffer.size() == HeaderEnd + 1) {
    Buffer.clear();
  } else {
    Buffer.erase(Buffer.begin(), Buffer.begin() + HeaderEnd);
  }
  return true;
}

bool Message::tryParseBody() {
  Data = std::make_shared<json>();
  try {
    *Data = json::parse(Buffer.begin(), Buffer.begin() + getLength());
  } catch (json::parse_error e) {
    *Data = json();
  }

  if (Buffer.size() == getLength()) {
    Buffer.clear();
  } else {
    Buffer.erase(Buffer.begin(), Buffer.begin() + getLength());
  }

  return !(*Data).is_null();
}

bool MessageReader::checkMessageHeader() {
  for (auto const &Element : Msg.getHeaders()) {
    if (Element.first.compare(Message::LENGTH_HEADER) == 0) {
      Msg.setLength(stoul(Element.second));
      return true;
    }
  }
  return false;
}

void SocketMessageReader::read() {
  asio::streambuf Buf;
  asio::read(Socket, asio::buffer(Msg.getBuffer()));

  if (!checkMessageHeader()) {
    error("No valid Content-Length found in header data.");
    return;
  }

  Msg.getBuffer().reserve(Msg.getLength());
  asio::read(Socket, asio::buffer(Msg.getBuffer()));

  if (!Msg.tryParseBody()) {
    error("Failed to parse json data.");
  }
  callback(Msg.getData());
}

void StdinMessageReader::read() {
  size_t PrevSize;
  while (true) {
    int C = std::cin.get();
    Msg.getBuffer().push_back(C);

    if (Msg.tryParseHeader()) {
      if (!checkMessageHeader()) {
        error("No valid Content-Length found in header data.");
        return;
      }
      break;
    }
  }

  
  PrevSize = Msg.getBuffer().size();
  Msg.getBuffer().reserve(Msg.getLength());
  std::istreambuf_iterator<char> iter(std::cin);
  std::copy_n(iter, Msg.getLength() - PrevSize,std::back_inserter(Msg.getBuffer()));
  // Move iterator one step a head to next char
  iter++;

  if (!Msg.tryParseBody()) {
    error("Failed to parse json data.");
    return;
  }
  callback(Msg.getData());
}