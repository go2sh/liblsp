#ifndef LSP_MESSAGEWRITER_H
#define LSP_MESSAGEWRITER_H

#include <json.hpp>
#include <memory>

using json = nlohmann::json;

namespace lsp {

typedef std::shared_ptr<json> JsonPtr;
/// Interface for writing sending messages
class MessageWriter {

public:
  /// Send a json message
  virtual void write(JsonPtr Data) = 0;
};
}; // namespace lsp
#endif