#ifndef LSP_ASIOMESSAGEHANDLER_H
#define LSP_ASIOMESSAGEHANDLER_H

#include <lsp/jsonrpc/MessageReader.h>
#include <lsp/jsonrpc/MessageWriter.h>
#include <lsp/jsonrpc/Message.h>

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
  std::size_t Length = 0;

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
      if (Length > 0) {
        asio::async_read(*ReadStream, Buffer,
                         asio::transfer_exactly(Length),
                         std::bind(&AsioMessageReader<AsioType>::handleBody,
                                   this, std::placeholders::_1,
                                   std::placeholders::_2));
      } else {
        ErrorEmitter.emit(Error(ErrorCode::ParseError,"Invalid length set for message."));
      }
      return;
    }

    // Try to parse a header element
    Length = MessageReader::getLength(Header);

    // Consume the data in the buffer and restart the read
    Buffer.consume(Bytes);
    asio::async_read_until(*ReadStream, Buffer, '\n',
                           std::bind(&AsioMessageReader<AsioType>::handleHeader,
                                     this, std::placeholders::_1,
                                     std::placeholders::_2));
  }

  void handleBody(asio::error_code ErrorCode, std::size_t Bytes) {
    std::istream Stream(&Buffer);
    MessagePtr Msg;

    try {
      Msg = Message::parse(Stream);
      MessageHandler(std::move(Msg));
    } catch (Error e) {
      ErrorEmitter.emit(e);
    }

    Buffer.consume(Bytes);
    asio::async_read_until(*ReadStream, Buffer, '\n',
                           std::bind(&AsioMessageReader<AsioType>::handleHeader,
                                     this, std::placeholders::_1,
                                     std::placeholders::_2));
  }
};

template <typename AsioType> class AsioMessageWriter : public MessageWriter {
  std::shared_ptr<AsioType> WriteStream;

  std::queue<MessagePtr> WriteQueue;
  std::mutex QueueMutex;
  std::mutex WriteMutex;

  std::string Header;
  asio::streambuf Buffer;

public:
  AsioMessageWriter(std::shared_ptr<AsioType> WriteStream)
      : WriteStream(WriteStream) {}

  virtual void write(MessagePtr Msg) {
    std::unique_lock<std::mutex> Lock(QueueMutex);
    WriteQueue.push(std::move(Msg));
    if (WriteQueue.size() == 1) {
      startWrite();
    }
  }

private:
  void startWrite() {
    MessagePtr Msg = std::move(WriteQueue.front());
    std::stringstream HeaderStream;
    std::ostream Stream(&Buffer);
    
    Stream << *Msg;
    HeaderStream << "Content-Length: " << Buffer.size() << "\r\n\r\n";
    Header = HeaderStream.str();

    asio::async_write(*WriteStream, asio::buffer(Header),
                      std::bind(&AsioMessageWriter::handleHeader, this,
                                std::placeholders::_1, std::placeholders::_2));
  }

  void handleHeader(asio::error_code ErrorCode, std::size_t Bytes) {
    asio::async_write(*WriteStream, Buffer,
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