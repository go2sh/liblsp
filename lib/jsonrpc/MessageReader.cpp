#include <functional>
#include <iostream>
#include <istream>
#include <iterator>
#include <string>

#include <lsp/jsonrpc/MessageReader.h>

using namespace lsp;

const char *LENGTH_HEADER = "Content-Length";

std::size_t MessageReader::getLength(const std::string &Header) {
  std::size_t Colon = Header.find(":");
  std::size_t Key = Header.find("Content-Length");

  if (Colon == std::string::npos || Key == std::string::npos || Key + 14 != Colon) {
    return 0;
  }

  try {
    return stoul(Header.substr(Colon+1));
  } catch(...) {
    return 0;
  }
}