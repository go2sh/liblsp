# Find the json include directory
# The following variables are set if json is found.
#  json_FOUND        - True when the json include directory is found.
#  json_INCLUDE_DIR  - The path to where the json include files are.
# If json is not found, json_FOUND is set to false.
include(FindPackageHandleStandardArgs)
include(ExternalProject)

find_path(json_INCLUDE_DIR
  NAMES nlohmann/json.hpp
  DOC "json library header files"
  )

if (NOT json_INCLUDE_DIR)
  ExternalProject_Add(json-ep
    URL https://github.com/nlohmann/json/archive/v3.3.0.tar.gz
    PREFIX "${PROJECT_BINARY_DIR}/extenal"
    CONFIGURE_COMMAND "" # Disable configure step
    BUILD_COMMAND "" # Disable build step
    INSTALL_COMMAND "" # Disable install step
    UPDATE_COMMAND "" # Disable update step: clones the project only once
    )
  
  # Specify include dir
  ExternalProject_Get_Property(json-ep source_dir)
  set(json_INCLUDE_DIR ${source_dir}/single_include)
endif()

add_library(json INTERFACE)
target_include_directories(json INTERFACE ${json_INCLUDE_DIR})

mark_as_advanced(json_INCLUDE_DIR)
find_package_handle_standard_args(json
  REQUIRED_VARS json_INCLUDE_DIR
  )