#ifndef LSP_LANGUAGECLIENT_H
#define LSP_LANGUAGECLIENT_H
#include "LSProtocol.h"
#include "MessageConnection.h"

namespace lsp {
class LanguageClient {

    public:
    virtual void logMessage(const LogMessageParams & Params) = 0;
};

class ProxyLanguageClient : public LanguageClient {
    MessageConnection &Connection;

public:
    ProxyLanguageClient(MessageConnection & Connection) : Connection(Connection) {}

    virtual void logMessage(const LogMessageParams & Params) {
        json Data = Params;
        Connection.notify(Data);
    }
};
};
#endif
