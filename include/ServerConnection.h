#ifndef LSP_SERVERCONNECTION_H
#define LSP_SERVERCONNECTION_H

#include <asio.hpp>
using asio::ip::tcp;

namespace lsp {

class ServerConnection {
    asio::io_service IoService;
    tcp::socket Socket;

    public:
    ServerConnection() {
        tcp
    };

    ServerConnection(char * Hostname, char *Port) {
        tcp::resolver resolver(IoService);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);
    };
};

}; // namespace lsp

#endif