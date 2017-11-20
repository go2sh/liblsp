#ifndef LSPROTOCOL_H
#define LSPROTOCOL_H

#include <json.hpp>
#include <list>
#include <string>

#ifndef HAS_CXX17
#include <optional.hpp>
using nonstd::nullopt;
using nonstd::optional;
#else
#include <optional>
using std::optional;
#endif

using json = nlohmann::json;

namespace lsp {

enum class TraceLevels { Off, Messages, Verbose };

enum class ErrorCode {
  ParseError = -32700,
  InvalidRequest = -32600,
  MethodNotFound = -32601,
  InvalidParams = -32602,
  InternalError = -32603,
  serverErrorStart = -32099,
  serverErrorEnd = -32000,
  ServerNotInitialized = -32002,
  UnknownErrorCode = -32001,

  // Defined by the protocol.
  RequestCancelled = -32800
};

struct EmptyResponse {
  json dump() const { return json(); }
};

template <typename T> struct ErrorResponse {
  ErrorCode Code;
  std::string Message;
  optional<T> Data;

  ErrorResponse(ErrorCode Code, const std::string &Message)
      : Code(Code), Message(Message) {}
  
  json dump() const {
    json Response;
    Response["code"] = Code;
    Response["message"] = Message;
    if (Data) {
      Response["data"] = Data.value().dump();
    }

    return Response;
  }
};
struct Position {
  uint32_t Line;
  uint32_t Character;

  void parse(json::object_t *Position);
  json dump();
};

struct Range {
  Position Start;
  Position End;

  void parse(json::object_t *Position);
  json dump();
};

struct Location {
  Location(const std::string &DocumentUri, const lsp::Range &Range)
      : DocumentUri(DocumentUri), Range(Range) {}

  std::string DocumentUri;
  lsp::Range Range;

  json dump();
};

struct InitializeParams {
  uint32_t ProcessId;
  std::string DocumentUri = "";
  TraceLevels TraceLevel;

  void parse(json::object_t *Params);
};

struct TextDocumentPositionParams {
  std::string DocumentUri;
  lsp::Position Position;

  void parse(json::object_t *Params);
};

struct MarkedString {
  MarkedString(const std::string &Value) : Value(Value){};
  MarkedString(const std::string &Value, const std::string &Language)
      : Value(Value), Language(Language){};

  std::string Value;
  std::string Language = "";

  json dump();
};

struct Hover {
  std::list<MarkedString> Contents;
  optional<lsp::Range> Range;

  json dump();
};

}; // namespace lsp

#endif // !LSPROTOCOL_H