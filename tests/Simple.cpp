#include "ServerConnection.h"


int main(int argc, char **argv) {
    int a = 1;
    while(a);
    lsp::StdioServerConnection Con;
    Con.run();
}