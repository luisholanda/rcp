cmake_minimum_required(VERSION 3.17)
project(rcp)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(INCLUDES_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/includes/)
file(GLOB HEADER_FILES "${INCLUDES_ROOT_FOLDER}/*.hh" "${INCLUDES_ROOT_FOLDER}/**/*.hh")

add_subdirectory(third-party/googletest)
add_subdirectory(src/rcp)
add_subdirectory(tests)

set_target_properties(rcp PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
