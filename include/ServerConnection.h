#ifndef LSP_SERVERCONNECTION_H
#define LSP_SERVERCONNECTION_H
#include <thread>

#include <asio.hpp>
using asio::ip::tcp;

#include "MessageConnection.h"
#include "MessageReader.h"

namespace lsp {

class ServerConnection {
  // asio::io_service IoService;
  // tcp::socket Socket;

public:
  ServerConnection() {}

  ServerConnection(char *Hostname, char *Port) {
    // tcp::resolver resolver(IoService);
    // tcp::resolver::query query(argv[1], argv[2]);
    // tcp::resolver::iterator iterator = resolver.resolve(query);
  }
};

class StdioServerConnection {
  asio::io_service Service;
  StdinMessageReader Reader;
  StdoutMessageWriter Writer;
  StdIoLogger Log;
  MessageConnection Connection;
  std::thread ConnectionThread;

public:
  StdioServerConnection() : Reader(), Connection(&Reader, &Writer, &Log) {}

  void run();
};

}; // namespace lsp

#endif