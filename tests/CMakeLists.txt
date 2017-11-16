include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Simple 
    Simple.cpp
)
target_link_libraries(Simple lsp)