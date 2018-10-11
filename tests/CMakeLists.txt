include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Simple 
    Simple.cpp
)

if (!${WIN32})
    set(LIBS pthread)
endif(!${WIN32})

set_property(TARGET Simple PROPERTY CXX_STANDARD 17)
target_link_libraries(Simple lsp ${LIBS})