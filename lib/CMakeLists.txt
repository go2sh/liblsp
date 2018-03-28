include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(lsp
    LSProtocol.cpp
    MessageHandler.cpp
    MessageConnection.cpp
    LanguageServer.cpp
    lsp.cpp
)

set_property(TARGET lsp PROPERTY CXX_STANDARD 17)