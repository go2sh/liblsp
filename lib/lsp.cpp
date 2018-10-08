#include "jsonrpc/AsioMessageHandler.h"
#include <lsp/lsp.h>

#include <asio.hpp>
#include <memory>

using namespace lsp; 
using asio::ip::tcp;

MessageConnection *lsp::createAsioTCPConnection(asio::io_service &io_service,
                                           const std::string &Hostname,
                                           const std::string &Port) {
  std::shared_ptr<tcp::socket> s = std::make_shared<tcp::socket>(io_service);
  tcp::resolver resolver(io_service);
  asio::connect(*s, resolver.resolve({Hostname, Port}));

  AsioMessageReader<tcp::socket> *Reader = new AsioMessageReader<tcp::socket>(s);
  AsioMessageWriter<tcp::socket> *Writer = new AsioMessageWriter<tcp::socket>(s);
  MessageConnection *Connection = new MessageConnection(Reader, Writer);

  return Connection;
}
