#ifndef LSP_ASIOMESSAGEHANDLER_H
#define LSP_ASIOMESSAGEHANDLER_H

#include <lsp/MessageReader.h>
#include <lsp/MessageWriter.h>

#include <asio.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>

using asio::error::basic_errors;

namespace lsp {

template <typename AsioType> class AsioMessageReader : public MessageReader {
  std::shared_ptr<AsioType> ReadStream;
  asio::streambuf Buffer;

public:
  AsioMessageReader(std::shared_ptr<AsioType> ReadStream)
      : ReadStream(ReadStream) {}

  virtual void listen(CallbackFunction Callback) {
    MessageHandler = Callback;
    asio::async_read_until(*ReadStream, Buffer, '\n',
                           std::bind(&AsioMessageReader<AsioType>::handleHeader,
                                     this, std::placeholders::_1,
                                     std::placeholders::_2));
  }

private:
  void handleHeader(asio::error_code ErrorCode, std::size_t Bytes) {
    std::string Header(asio::buffers_begin(Buffer.data()),
                       asio::buffers_begin(Buffer.data()) + Bytes);

    if (ErrorCode) {
      switch (ErrorCode.value()) {
      case basic_errors::connection_aborted:
      case basic_errors::connection_reset:
      case basic_errors::timed_out:
      case basic_errors::shut_down:
        CloseEmitter.emit();
        return;
      default:
        break;
      }
    }

    // Handle end of header event
    if (Header.compare("\n") == 0 || Header.compare("\r\n") == 0) {
      Buffer.consume(Bytes);
      if (CurrentMessage.getLength() > 0) {
        asio::async_read(*ReadStream, Buffer,
                         asio::transfer_exactly(CurrentMessage.getLength()),
                         std::bind(&AsioMessageReader<AsioType>::handleBody,
                                   this, std::placeholders::_1,
                                   std::placeholders::_2));
      } else {
        ErrorEmitter.emit("Invalid length set for message.");
      }
      return;
    }

    // Try to parse a header element
    if (!CurrentMessage.parseHeader(Header)) {
      ErrorEmitter.emit("Invalid header received: " + Header);
    }

    // Consume the data in the buffer and restart the read
    Buffer.consume(Bytes);
    asio::async_read_until(*ReadStream, Buffer, '\n',
                           std::bind(&AsioMessageReader<AsioType>::handleHeader,
                                     this, std::placeholders::_1,
                                     std::placeholders::_2));
  }

  void handleBody(asio::error_code ErrorCode, std::size_t Bytes) {
    std::string Body(asio::buffers_begin(Buffer.data()),
                     asio::buffers_begin(Buffer.data()) + Bytes);

    if (!CurrentMessage.parseBody(Body)) {
      ErrorEmitter.emit("Failed to parse json body");
    } else {
      MessageHandler(CurrentMessage.getData());
    }

    Buffer.consume(Bytes);
    CurrentMessage = Message();
    asio::async_read_until(*ReadStream, Buffer, '\n',
                           std::bind(&AsioMessageReader<AsioType>::handleHeader,
                                     this, std::placeholders::_1,
                                     std::placeholders::_2));
  }
};

template <typename AsioType> class AsioMessageWriter : public MessageWriter {
  std::shared_ptr<AsioType> WriteStream;

  std::queue<JsonPtr> WriteQueue;
  std::mutex QueueMutex;
  std::mutex WriteMutex;

  std::string Header;
  std::string Data;

public:
  AsioMessageWriter(std::shared_ptr<AsioType> WriteStream)
      : WriteStream(WriteStream) {}

  virtual void write(JsonPtr JsonData) {
    std::unique_lock<std::mutex> Lock(QueueMutex);
    WriteQueue.push(JsonData);
    if (WriteQueue.size() == 1) {
      startWrite();
    }
  }

private:
  void startWrite() {
    std::stringstream HeaderStream;
    JsonPtr JsonData = WriteQueue.front();

    Data = JsonData->dump();
    HeaderStream << "Content-Length: " << Data.size() << "\r\n\r\n";
    Header = HeaderStream.str();

    asio::async_write(*WriteStream, asio::buffer(Header),
                      std::bind(&AsioMessageWriter::handleHeader, this,
                                std::placeholders::_1, std::placeholders::_2));
  }

  void handleHeader(asio::error_code ErrorCode, std::size_t Bytes) {
    asio::async_write(*WriteStream, asio::buffer(Data),
                      std::bind(&AsioMessageWriter::handleBody, this,
                                std::placeholders::_1, std::placeholders::_2));
  }

  void handleBody(asio::error_code ErrorCode, std::size_t Bytes) {
    std::unique_lock<std::mutex> Lock(QueueMutex);
    WriteQueue.pop();
    if (!WriteQueue.empty()) {
      startWrite();
    }
  }
};
} // namespace lsp

#endif