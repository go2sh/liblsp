#ifndef LSP_SOCKETCONNECTION_H
#define LSP_SOCKETCONNECTION_H
#include <memory>
#include <set>
#include <utility>

#include <asio.hpp>
#include "MessageReader.h"

using asio::ip::tcp;

namespace lsp {
class SocketConnection;
typedef std::shared_ptr<SocketConnection> SocketConnectionPtr;

class ConnectionManager {
public:
    void addConnection(SocketConnectionPtr Connection) {
        Connections.insert(Connection);
    }

    void removeConnection(SocketConnectionPtr Connection) {
        Connections.erase(ConnectionPtr);
    }
private:
    std::set<SocketConnectionPtr> Connections;
};

class SocketConnection : public std::enable_shared_from_this<SocketConnection> {
  tcp::socket Socket;
  SocketMessageReader Reader;
  SocketMessageWriter Writer;
  ConnectionManager *Manager;


public:
  SocketConnection(tcp::socket Socket, ConnectionManager *Manager)
      : Socket(std::move(Socket)), Manager(Manager){};
  void start();
  tcp::Socket &getSocket() {return Socket};

private:
  void onData();
  bool tryReadHeader();
  bool tryReadData();
};

class SocketServer {
public:
  SocketServer(asio::io_service &IoService, const tcp::endpoint &Endpoint)
      : Acceptor(IoService, Endpoint), Socket(IoService) {
    do_accept();
  }

private:
  void do_accept() {
    Acceptor.async_accept(Socket, [this](std::error_code ec) {
      if (!ec) {
        std::make_shared<SocketConnection>(std::move(Socket), &this->Manager)->start();
      }
      do_accept();
    });
  }

  tcp::acceptor Acceptor;
  tcp::socket Socket;
  ConnectionManager Manager;
};
}; // namespace lsp

#endif