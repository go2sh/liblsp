#include <lsp/MessageReader.h>
#include <lsp/MessageWriter.h>

#include <iostream>
#include <string>

namespace lsp {
class StdinMessageReader : public MessageReader {
  std::string Header;
  std::string Body;

public:
  virtual void read() {
    while (true) {
      std::getline(std::cin, Header, '\n');
      if (std::cin.eof()) {
        CloseEmitter.emit();
      } else if (std::cin.bad()) {
        ErrorEmitter.emit("Failed to receive header.");
        CloseEmitter.emit();
      }

      if (Header.length() == 0 || Header.compare("\r") == 0) {
        break;
      }

      // Try to parse a header element
      if (!CurrentMessage.parseHeader(Header)) {
        ErrorEmitter.emit("Invalid header received: " + Header);
      }
    }

    // Clear string and reserve data
    Body.erase();
    Body.reserve(CurrentMessage.getLength());
    // Copy data
    std::istreambuf_iterator<char> iter(std::cin);
    std::copy_n(iter, CurrentMessage.getLength(), std::back_inserter(Body));
    if (std::cin.eof()) {
      CloseEmitter.emit();
    } else if (std::cin.bad()) {
      ErrorEmitter.emit("Failed to receive body.");
      CloseEmitter.emit();
      return;
    } else {
      // Move iterator one step a head to next char
      iter++;
    }

    if (!CurrentMessage.parseBody(Body)) {
      ErrorEmitter.emit("Failed to parse json body");
      return;
    }

    MessageHandler(CurrentMessage.getData());
    CurrentMessage = Message();
  }
};

class StdoutMessageWriter : public MessageWriter {
public:
  virtual void write(JsonPtr JsonData) {
    std::string Buf = Data->dump();

    std::cout << "Content-Length: " << Buf.size();
    std::cout.write("\r\n\r\n", 4);

    std::cout.write(Buf.c_str(), Buf.size());
    std::cout.flush();
  }
};
}