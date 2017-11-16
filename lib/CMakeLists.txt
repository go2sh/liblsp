include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(lsp
    LSProtocol.cpp
    MessageReader.cpp
    MessageConnection.cpp
)

set_property(TARGET lsp PROPERTY CXX_STANDARD 17)