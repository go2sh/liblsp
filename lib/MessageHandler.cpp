#include "lsp/MessageReader.h"

using namespace lsp;
using json = nlohmann::json;

bool Message::parseHeader(const std::string &Header) {
  std::size_t ColonPos = Header.find(":");

  // Check for colon in header string
  if (ColonPos == std::string::npos) {
    return false;
  }

  // Get field and value substrings
  std::string Field = Header.substr(0, ColonPos);
  std::string Value = Header.substr(ColonPos + 1);

  // Check for message length
  if (Field.compare("Content-Length")==0) {
    try {
      Length = stoul(Value);
      return true;
    } catch (std::exception E) {
      return false;
    }
  }

  return false;
}

bool Message::parseBody(const std::string &Body) {
  Data = std::make_shared<json>();
  
  try {
    *Data = json::parse(Body);
  } catch (json::parse_error e) {
    *Data = json();
  }

  return !(*Data).is_null();
}