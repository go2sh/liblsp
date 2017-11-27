#ifndef LSPROTOCOL_H
#define LSPROTOCOL_H

#include <json.hpp>
#include <list>
#include <optional>
#include <string>

template <typename T> using optional = std::optional<T>;

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

  friend void from_json(const json &Data, Position &P) {
    P.Line = Data.at("line");
    P.Character = Data.at("character");
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

  Diagnostic() {}
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

struct PublishDiagnosticsParams {
  std::string Uri;
  std::vector<Diagnostic> Diagnostics;

  friend void to_json(json &Data, const PublishDiagnosticsParams &PDP) {
    Data["uri"] = PDP.Uri;
    Data["diagnostics"] = PDP.Diagnostics;
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

  friend void from_json(const json &Data, TextDocumentIdentifier &I) {
    I.Uri = Data.at("uri");
  }
};

struct VersionedTextDocumentIdentifier : public TextDocumentIdentifier {
  uint32_t Version;

  friend void to_json(json &Data, const VersionedTextDocumentIdentifier &V) {
    Data["uri"] = V.Uri;
    Data["version"] = V.Version;
  }

  friend void from_json(const json &Data, VersionedTextDocumentIdentifier &V) {
    V.Uri = Data.at("uri");
    V.Version = Data.at("version");
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

struct ClientCapabilities {

  friend void from_json(const json &Data, ClientCapabilities &CC) {}
};
struct EmptyInitializationOptions {
  friend void from_json(const json &Data,
                        EmptyInitializationOptions &Options){};
};

template <typename T> struct InitializeParams {
  uint32_t ProcessId;
  std::string RootUri = "";
  TraceLevels TraceLevel = TraceLevels::Off;
  optional<T> InitializationOptions;
  ClientCapabilities Capabilities;

  friend void from_json(const json &Data, InitializeParams<T> &Params) {
    Params.ProcessId = Data.at("processId").get<uint32_t>();
    auto Uri = Data.find("rootUri");
    if (Uri != Data.end()) {
      if ((*Uri).is_null()) {
        Params.RootUri = "";
      } else {
        Params.RootUri = *Uri;
      }
    } else {
      auto Path = Data.find("rootPath");
      if (Path != Data.end()) {
        if ((*Path).is_null()) {
          Params.RootUri = "";
        } else {
          Params.RootUri = *Path;
        }
      }
    }

    auto InitOptions = Data.find("initializationOptions");
    if (InitOptions != Data.end())
      Params.InitializationOptions = *InitOptions;
    Params.Capabilities = Data.at("capabilities");
    auto TraceLevel = Data.find("traceLevel");
    if (TraceLevel != Data.end())
      Params.TraceLevel = *TraceLevel;
  }
};

enum class TextDocumentSyncKind { None = 0, Full = 1, Incremental = 2 };

struct SaveOptions {
  bool IncluedeText = false;

  friend void to_json(json &Data, const SaveOptions &O);
};
void to_json(json &Data, const SaveOptions &O);

struct TextDocumentSyncOptions {
  bool OpenClose;
  TextDocumentSyncKind Change;
  bool WillSave;
  bool WillSaveWaitUntil;
  SaveOptions Save;

  friend void to_json(json &Data, const TextDocumentSyncOptions &O);
};
void to_json(json &Data, const TextDocumentSyncOptions &O);

struct CompletionOptions {
  bool ResolveProvider = false;
  std::vector<std::string> TriggerCharacters;

  friend void to_json(json &Data, const TextDocumentSyncOptions &O);
};
void to_json(json &Data, const CompletionOptions &O);

struct ServerCapabilities {
  TextDocumentSyncOptions TextDocumentSync;
  bool HoverProvider;
  CompletionOptions CompletionProvider;

  friend void to_json(json &Data, const ServerCapabilities &SC) {
    Data["textDocumentSync"] = SC.TextDocumentSync;
    Data["hoverProvider"] = SC.HoverProvider;
    Data["completionProvider"] = SC.CompletionProvider;
  }
};

struct InitializeResult {
  ServerCapabilities Capabilities;

  friend void to_json(json &Data, const InitializeResult &IR) {
    Data["capabilities"] = IR.Capabilities;
  }
};

enum class MessageType { Error = 1, Warning = 2, Info = 3, Log = 4 };

struct LogMessageParams {
  MessageType type;
  std::string message;

  friend void to_json(json &Data, const LogMessageParams &Params);
  friend void from_json(const json &Data, LogMessageParams &Params);
};

void to_json(json &Data, const LogMessageParams &Params);
void from_json(const json &Data, LogMessageParams &Params);

struct TextDocumentPositionParams {
  lsp::TextDocumentIdentifier TextDocument;
  lsp::Position Position;

  friend void from_json(const json &Params, TextDocumentPositionParams &TDP);
};
void from_json(const json &Params, TextDocumentPositionParams &TDP);

enum class MarkupKind { Plaintext, Markdown };

struct MarkupContent {
  MarkupContent() {}
  MarkupContent(const std::string &Value) : Value(Value){};
  MarkupContent(const std::string &Value, const std::string &Language)
      : Value(Value), Language(Language){};

  std::string Value = "";
  std::string Language = "";
  lsp::MarkupKind Kind = lsp::MarkupKind::Plaintext;

  friend void to_json(json &Data, const MarkupContent &MC) {
    Data["value"] = MC.Value;
    Data["language"] = MC.Language;
  }
};

struct Hover {
  lsp::MarkupContent Contents;
  optional<lsp::Range> Range;

  Hover() {}

  friend void to_json(json &Data, const Hover &H) {
    Data["contents"] = H.Contents;
    if (H.Range.has_value()) {
      Data["range"] = H.Range.value();
    }
  }
};

enum class CompletionTriggerKind { Invoked = 1, TriggerCharacter = 2 };

enum class InsertTextFormat { PlainText = 1, Snippet = 2 };

struct CompletionContext {
  CompletionTriggerKind TriggerKind;
  std::string TriggerCharacter;

  friend void from_json(const json &Data, CompletionContext &CC);
};
void from_json(const json &Data, CompletionContext &CC);

struct CompletionParams : public TextDocumentPositionParams {
  optional<CompletionContext> Context;
  
  friend void from_json(const json &Data, CompletionParams &CP);
};
void from_json(const json &Data, CompletionParams &CP);

struct EmptyCompletionData {};

template <typename T> struct CompletionItem {
  std::string Label;
  uint32_t Kind = 1;

  optional<std::string> Detail;
  optional<lsp::MarkupContent> Documentation;
  optional<std::string> SortText;
  optional<std::string> FilterText;
  optional<std::string> InsertText;
  lsp::InsertTextFormat InsertTextFormat;
  optional<lsp::TextEdit> TextEdit;
  std::vector<lsp::TextEdit> AdditionalTextEdits;
  std::vector<std::string> CommitCharacters;
  lsp::Command Command;
  T Data;

  friend void to_json(json &Data, const CompletionItem<EmptyCompletionData> &Item);
};
void to_json(json &Data, const CompletionItem<EmptyCompletionData> &Item);

}; // namespace lsp

#endif // !LSPROTOCOL_H