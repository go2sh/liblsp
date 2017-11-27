#include <iostream>
#include <string>

#include "MessageWriter.h"

using namespace lsp;

void StdoutMessageWriter::write(json &Data) {
    std::string Buf = Data.dump();
    
    std::cout << "Content-Length: " << Buf.size();
    std::cout.write("\r\n\r\n",4);

    std::cout.write(Buf.c_str(),Buf.size());
    std::cout.flush();
}