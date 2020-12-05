file(GLOB RCP_SRC
  ${HEADER_FILES}
  "*.cc"
  "**/*.cc")

add_library(rcp STATIC ${RCP_SRC})
target_include_directories(rcp PRIVATE ${INCLUDES_ROOT_FOLDER})
