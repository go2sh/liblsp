#include <json.hpp>

#include "MessageConnection.h"

using json = nlohmann::json;
using namespace lsp;

void MessageConnection::errorHandler(const std::string &Message) {
    Log->logError(Message);
}

void MessageConnection::messageHandler(json &Message) {

};