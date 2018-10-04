add_library(lsp
    LSProtocol.cpp
    MessageHandler.cpp
    MessageConnection.cpp
    LanguageServer.cpp
    lsp.cpp
)

target_link_libraries(lsp json asio)

set_property(TARGET lsp PROPERTY CXX_STANDARD 17)