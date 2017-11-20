#include <thread>

#include "ServerConnection.h"

using namespace lsp;

void StdioServerConnection::run() {
  ConnectionThread = std::thread([this] { Connection.run(); });
  while (true) {
    Connection.processMessageQueue();
  }

  ConnectionThread.join();
}
