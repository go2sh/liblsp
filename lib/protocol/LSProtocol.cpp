#include <lsp/jsonrpc/Message.h>
#include <lsp/protocol/LSProtocol.h>

using namespace lsp;

RequestType<InitializeParams<std::string>, InitializeResult>
    lsp::InitializeRequest("initialize");

void Position::parse(json::object_t *Position) {
  if (Position->find("line") != Position->end()) {
    if ((*Position)["line"].is_number_unsigned()) {
      Line = (*Position)["line"];
    }
  }
  if (Position->find("character") != Position->end()) {
    if ((*Position)["line"].is_number_unsigned()) {
      Character = (*Position)["character"];
    }
  }
}

json Position::dump() {
  json J;
  J["line"] = Line;
  J["character"] = Character;
  return J;
}

void Range::parse(json::object_t *Position) {
  if (Position->find("start") != Position->end()) {
    if ((*Position)["start"].is_object()) {
      Start.parse((*Position)["start"].get_ptr<json::object_t *>());
    }
  }
  if (Position->find("end") != Position->end()) {
    if ((*Position)["end"].is_object()) {
      End.parse((*Position)["end"].get_ptr<json::object_t *>());
    }
  }
}

json Range::dump() {
  json J;
  J["start"] = Start.dump();
  J["end"] = End.dump();
  return J;
}

json Location::dump() {
  json J;
  J["uri"] = Uri;
  J["range"] = Range.dump();
  return J;
}
void lsp::to_json(json &Data, const LogMessageParams &Params) {
  Data["message"] = Params.message;
  Data["type"] = Params.type;
}

void lsp::from_json(const json &Data, LogMessageParams &Params) {
  //TODO:
}

NotificationType<LogMessageParams> ShowMessage("window/showMessage");

void lsp::from_json(const json &Data, TextDocumentItem &Item) {
  Item.Uri = Data.at("uri");
  Item.LanguageId = Data.at("languageId");
  Item.Version = Data.at("version");
  Item.Text = Data.at("text");
}

void lsp::from_json(const json &Params, TextDocumentPositionParams &TDP) {
  TDP.TextDocument = Params.at("textDocument");
  TDP.Position = Params.at("position");
}

void lsp::to_json(json &Data, const SaveOptions &O) {
  Data["includeText"] = O.IncluedeText;
}

void lsp::to_json(json &Data, const TextDocumentSyncOptions &O) {
  Data["openClose"] = O.OpenClose;
  Data["change"] = O.Change;
  Data["willSave"] = O.WillSave;
  Data["willSaveWaitUntil"] = O.WillSaveWaitUntil;
  Data["save"] = O.Save;
}

/*
 * Workspace sync
 */

void lsp::from_json(const json &Params, TextDocumentDidOpenParams &TDO) {
  TDO.TextDocument = Params.at("textDocument");
}
NotificationType<TextDocumentDidOpenParams>
    lsp::TextDocumentDidOpen("textDocument/didOpen");

void lsp::from_json(const json &Data, TextDocumentContentChangeEvent &CCE) {
  auto Range = Data.find("range");
  if (Range != Data.end()) {
    CCE.Range = *Range;
  }
  auto RL = Data.find("rangeLength");
  if (RL != Data.end()) {
    CCE.RangeLength = (*RL).get<uint32_t>();
  }
  CCE.Text = Data.at("text");
}

void lsp::from_json(const json &Params, TextDocumentDidChangeParams &DCP) {
  DCP.TextDocument = Params.at("textDocument");
  DCP.ContentChanges =
      Params.at("contentChanges")
          .get<std::vector<lsp::TextDocumentContentChangeEvent>>();
}
NotificationType<TextDocumentDidChangeParams>
    lsp::TextDocumentDidChange("textDocument/didChange");

void lsp::to_json(json &Data, const CompletionOptions &O) {
  Data["resolveProvider"] = O.ResolveProvider;
  Data["triggerCharacters"] = O.TriggerCharacters;
}

void lsp::from_json(const json &Data, CompletionContext &CC) {
  CC.TriggerKind = Data.at("triggerKind");
  auto TC = Data.find("triggerCharacter");
  if (TC != Data.end()) {
    CC.TriggerCharacter = *TC;
  }
}
void lsp::from_json(const json &Data, CompletionParams &CP) {
  CP.TextDocument = Data.at("textDocument");
  CP.Position = Data.at("position");
  auto Context = Data.find("context");
  if (Context != Data.end()) {
    CP.Context = *Context;
  }
}

void lsp::to_json(json &Data, const CompletionItem<EmptyCompletionData> &Item) {
  Data["label"] = Item.Label;
  Data["kind"] = Item.Kind;
}

/*
 * Language Features
 */
void lsp::to_json(json &Data, const FormattingOptions &Item) {
  Data["tabSize"] = Item.TabSize;
  Data["insertSpaces"] = Item.InsertSpaces;
}

void lsp::from_json(const json &Data, FormattingOptions &Item) {
  Item.TabSize = Data.at("tabSize");
  Item.InsertSpaces = Data.at("insertSpaces");
}

void lsp::to_json(json &Data, const DocumentFormattingParams &Item) {
  Data["textDocument"] = Item.TextDocument;
  Data["options"] = Item.Options;
}

void lsp::from_json(const json &Data, DocumentFormattingParams &Item) {
  Item.TextDocument = Data.at("textDocument");
  Item.Options = Data.at("options");
}
RequestType<DocumentFormattingParams, std::vector<TextEdit>>
    lsp::TextDocumentFormatting("textDocument/formatting");