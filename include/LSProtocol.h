#ifndef LSPROTOCOL_H
#define LSPROTOCOL_H

#include <string>
#include <list>
#include <json.hpp>

#ifndef HAS_CXX17
#include <optional.hpp>
using nonstd::optional;
using nonstd::nullopt;
#else
#include <optional>
using std::optional;
#endif

using json = nlohmann::json;

namespace lsp {

enum class TraceLevels {
    Off,
    Messages,
    Verbose
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
    Location(const std::string &DocumentUri, const lsp::Range &Range) : DocumentUri(DocumentUri), Range(Range) {}

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
    MarkedString(const std::string & Value) : Value(Value) {};
    MarkedString(const std::string & Value, const std::string & Language) : Value(Value), Language(Language) {};

    std::string Value;
    std::string Language = "";

    json dump();
};

struct Hover {
    std::list<MarkedString> Contents;
    optional<lsp::Range> Range;

    json dump();
};

};

#endif // !LSPROTOCOL_H