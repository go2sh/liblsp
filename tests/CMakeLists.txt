include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Simple 
    Simple.cpp
)
set_property(TARGET Simple PROPERTY CXX_STANDARD 17)
target_link_libraries(Simple lsp pthread)