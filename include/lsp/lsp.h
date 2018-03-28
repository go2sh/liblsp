#ifndef LSP_LSP_H
#define LSP_LSP_H

#include <lsp/MessageConnection.h>

#include <asio.hpp>

namespace lsp {

MessageConnection *createStdIoConnection();
MessageConnection *createAsioTCPConnection(asio::io_service &io_service,
                                           const std::string &Hostname,
                                           const std::string &Port);

}; // namespace lsp
#endif