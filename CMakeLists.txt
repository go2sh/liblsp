cmake_minimum_required(VERSION 3.9)

project(liblsp)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/ )

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

if (UNIX)
find_package (Threads REQUIRED)
link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

find_package(Asio REQUIRED)
find_package(json REQUIRED)

# Add include directories
include_directories(include/)


add_subdirectory(lib/)
add_subdirectory(tests/)

