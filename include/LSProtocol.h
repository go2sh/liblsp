#ifndef LSPROTOCOL_H
#define LSPROTOCOL_H

#include <json.hpp>
#include <list>
#include <string>
#include <optional>


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

  friend void to_json(json &Data, const Position &P) {
    Data["line"] = P.Line;
    Data["character"] = P.Character;
  }

  void parse(json::object_t *Position);
  json dump();
};

struct Range {
  Position Start;
  Position End;

  friend void to_json(json &Data, const Range &R) {
    Data["start"] = R.Start;
    Data["end"] = R.End;
  }

  void parse(json::object_t *Position);
  json dump();
};

struct Location {
  Location(const std::string &Uri, const lsp::Range &Range)
      : Uri(Uri), Range(Range) {}

  std::string Uri;
  lsp::Range Range;

  friend void to_json(json &Data, const Location &L) {
    Data["uri"] = L.Uri;
    Data["range"] = L.Range;
  }

  json dump();
};

enum class DiagnosticSeverity {
  Error = 1,
  Warning = 2,
  Information = 3,
  Hint = 4
};

struct Diagnostic {
  lsp::Range Range;
  std::optional<DiagnosticSeverity> Severity;
  std::optional<std::string> Code;
  std::optional<std::string> Source;
  std::string Message;

  Diagnostic(const lsp::Range &Range, const std::string &Message)
      : Range(Range), Message(Message) {}
  Diagnostic(const lsp::Range &Range, const std::string &Message,
             lsp::DiagnosticSeverity Severity)
      : Range(Range), Message(Message), Severity(Severity) {}

  friend void to_json(json &Data, const Diagnostic &Diag) {
    Data = json();
    Data["range"] = Diag.Range;
    Data["message"] = Diag.Message;
    if (Diag.Severity) {
      Data["severity"] =
          static_cast<std::underlying_type_t<lsp::DiagnosticSeverity>>(
              *Diag.Severity);
    }
    if (Diag.Code) {
      Data["code"] = *Diag.Code;
    }
    if (Diag.Source) {
      Data["source"] = *Diag.Source;
    }
  }
};

struct Command {
  std::string Title;
  std::string Cmd;
  std::optional<std::vector<std::string>> Arguments;

  friend void to_json(json &Data, const Command &C) {
    Data["title"] = C.Title;
    Data["command"] = C.Cmd;
    // TODO: Args
  }
};

struct TextEdit {
  lsp::Range Range;
  std::string NewText;

  friend void to_json(json &Data, const TextEdit &TE) {
    Data["range"] = TE.Range;
    Data["newText"] = TE.NewText;
  }
};

struct TextDocumentIdentifier {
  std::string Uri;

  friend void to_json(json &Data, const TextDocumentIdentifier &I) {
    Data["uri"] = I.Uri;
  }
};

struct VersionedTextDocumentIdentifier {
  uint32_t Version;

  friend void to_json(json &Data, const VersionedTextDocumentIdentifier &V) {
    Data["version"] = V.Version;
  }
};

struct TextDocumentEdit {
  lsp::VersionedTextDocumentIdentifier TextDocument;
  std::vector<lsp::TextEdit> Edits;

  friend void to_json(json &Data, const TextDocumentEdit &Edit) {
    Data["textDocument"] = Edit.TextDocument;
    Data["edits"] = Edit.Edits;
  }
};

struct WorkspaceEdit {
  std::optional<std::map<std::string, std::vector<lsp::TextEdit>>> Changes;
  std::optional<std::vector<lsp::TextDocumentEdit>> DocumentChanges;

  friend void to_json(json &Data, const WorkspaceEdit &WE) {
    if (WE.Changes) {
      Data["changes"] = *WE.Changes;
    }
    if (WE.DocumentChanges) {
      Data["documentChanges"] = *WE.DocumentChanges;
    }
  }
};

struct TextDocumentItem {
  std::string Uri;
  std::string LanguageId;
  uint32_t Version;
  std::string Text;

  friend void to_json(json &Data, const TextDocumentItem &Item) {
    Data["uri"] = Item.Uri;
    Data["languageId"] = Item.LanguageId;
    Data["version"] = Item.Version;
    Data["text"] = Item.Text;
  }
};

struct DocumentFilter {
  std::optional<std::string> Language;
  std::optional<std::string> Scheme;
  std::optional<std::string> Pattern;

  friend void to_json(json &Data, const DocumentFilter &Filter) {
    if (Filter.Language) {
      Data["language"] = *Filter.Language;
    }
    if (Filter.Scheme) {
      Data["scheme"] = *Filter.Scheme;
    }
    if (Filter.Pattern) {
      Data["pattern"] = *Filter.Pattern;
    }
  }
};

struct InitializeParams {
  uint32_t ProcessId;
  std::string DocumentUri = "";
  TraceLevels TraceLevel;

  void parse(json::object_t *Params);
};

enum class MessageType { Error = 1, Warning = 2, Info = 3, Log = 4 };

struct LogMessageParams {
  MessageType type;
  std::string message;

  friend void to_json(json &Data, const LogMessageParams &Params);
  friend void from_json(const json &Data, LogMessageParams &Params);
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