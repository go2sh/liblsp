#include <json.hpp>
#include <string>

using json = nlohmann::json;

#include "LSProtocol.h"

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
      Start.parse((*Position)["start"].get_ptr<json::object_t*>());
    }
  }
  if (Position->find("end") != Position->end()) {
    if ((*Position)["end"].is_object()) {
      End.parse((*Position)["end"].get_ptr<json::object_t*>());
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
  J["uri"] = DocumentUri;
  J["range"] = Range.dump();
  return J;
}

json MarkedString::dump() {
  json J;
  if (Language != "" ) {
    J["language"] = Language;
    J["value"] = Value;
  } else {
    J = Value;
  }
  return J;
}


json Hover::dump() {
  json J;
  if (Range) {
    J["range"] = Range.value().dump();
  }
  if (Contents.size() >= 1) {
    json::array_t Dumps;
    std::for_each(Contents.begin(),Contents.end(),[&Dumps] (MarkedString &String) {Dumps.push_back(String.dump());});
    J["contents"] = Dumps;
  }
  return J;
}

void InitializeParams::parse(json::object_t *Params) {
  for (auto &Param : *Params) {
    auto Key = Param.first;
    auto Value = Param.second;

    if (Key == "processId") {
      auto Value = Param.second;
      if (Value.is_number()) {
        ProcessId = Value;
      } else if (Value.is_null()) {
        ProcessId = 0;
      }
    }

    if (Key == "documentUri") {
      if (Value.is_string()) {
        DocumentUri = Value.get<std::string>();
      } else if (Value.is_null()) {
        DocumentUri = "";
      } else {
      }
    } else if (Key == "rootPath") {
      if (Value.is_string()) {
        DocumentUri = Value.get<std::string>();
      } else if (Value.is_null()) {
        DocumentUri = "";
      } else {
      }
    }

    if (Key == "trace") {
      if (Value.is_string()) {
        if (Value == "off")
          TraceLevel = TraceLevels::Off;
        if (Value == "messages")
          TraceLevel = TraceLevels::Messages;
        if (Value == "verbose")
          TraceLevel = TraceLevels::Verbose;
      } else {
      }
    }
  }
}

void TextDocumentPositionParams::parse(json::object_t *Params) {
  if (Params->find("textDocument") != Params->end()) {
    if ((*Params)["textDocument"].is_object()) {
      json::object_t *TextDocument = (*Params)["textDocument"].get_ptr<json::object_t*>();
      if (TextDocument->find("uri") != TextDocument->end()) {
        if ((*TextDocument)["uri"].is_string()) {
          DocumentUri = (*TextDocument)["uri"].get<std::string>();
        }
      }
    }
  }
  if (Params->find("position") != Params->end()) {
    if ((*Params)["position"].is_object()) {
      Position.parse((*Params)["position"].get_ptr<json::object_t*>());
    }
  }
}