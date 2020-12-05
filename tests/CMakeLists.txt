set(BINARY ${CMAKE_PROJECT_NAME}_test)
file(GLOB TEST_SOURCES
  "*.hh"
  "*.cc"
  "**/*.cc")

add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC rcp gtest)
target_include_directories(${BINARY} PRIVATE ${INCLUDES_ROOT_FOLDER})
