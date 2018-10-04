# Find the asio include directory
# The following variables are set if asio is found.
#  asio_FOUND        - True when the asio include directory is found.
#  asio_INCLUDE_DIR  - The path to where the asio include files are.
# If asio is not found, asio_FOUND is set to false.
include(FindPackageHandleStandardArgs)
include(ExternalProject)

find_path(asio_INCLUDE_DIR
  NAMES asio.hpp
  DOC "asio library header files"
  )

if (NOT asio_INCLUDE_DIR)
  ExternalProject_Add(asio-ep
    URL https://downloads.sourceforge.net/project/asio/asio/1.12.1%20%28Stable%29/asio-1.12.1.zip
    TIMEOUT 5
    PREFIX "${PROJECT_BINARY_DIR}/extenal"
    CONFIGURE_COMMAND "" # Disable configure step
    BUILD_COMMAND "" # Disable build step
    INSTALL_COMMAND "" # Disable install step
    UPDATE_COMMAND "" # Disable update step: clones the project only once
    )
  
  # Specify include dir
  ExternalProject_Get_Property(asio-ep source_dir)
  set(asio_INCLUDE_DIR ${source_dir}/include)
endif()

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_INCLUDE_DIR})
target_compile_definitions(asio INTERFACE -DASIO_STANDALONE)
if (WIN32)
  target_compile_definitions(asio INTERFACE -D_WIN32_WINNT=0x0601)
endif()

mark_as_advanced(asio_INCLUDE_DIR)
find_package_handle_standard_args(asio
  REQUIRED_VARS asio_INCLUDE_DIR
  )