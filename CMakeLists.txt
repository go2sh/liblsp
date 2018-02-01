cmake_minimum_required(VERSION 3.9)

project(liblsp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/ )

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if (COMPILER_SUPPORTS_CXX17)
  add_definitions(-DHAS_CXX17)
endif()
set(CXX_STANDARD_REQUIRED 11)
set(CXX_STANDARD 17)


if (DEFINED ASIO_DIRECTORY)
  add_definitions(-DASIO_STANDALONE)
  if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
  endif()
  include_directories(${ASIO_DIRECTORY}/include)
else()
find_package(Boost COMPONENTS system regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
endif()
if (UNIX)
find_package (Threads REQUIRED)
link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

find_package(json REQUIRED)
include_directories(${json_INCLUDE_DIR})

add_subdirectory(lib/)
add_subdirectory(tests/)

