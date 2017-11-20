#ifndef LSP_MESSAGEWRITER_H
#define LSP_MESSAGEWRITER_H

#include <json.hpp>

using json = nlohmann::json;

namespace lsp {
class MessageWriter {

public:
    virtual void write(json & Data) = 0;
};

class StdoutMessageWriter : public MessageWriter {
    public:
    virtual void write(json & Data);
};
}; // namespace lsp
#endif