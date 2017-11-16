#include "ServerConnection.h"


int main(int argc, char **argv) {
    lsp::StdioServerConnection Con;
    Con.run();
}