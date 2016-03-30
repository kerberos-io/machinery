message("External project: Easylogging")

ExternalProject_Add(easylogging
  GIT_REPOSITORY ${git_protocol}://github.com/easylogging/easyloggingpp
  GIT_TAG v8.91
  SOURCE_DIR easylogging
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

set(EASYLOGGING_INCLUDE_DIR ${CMAKE_BINARY_DIR}/easylogging/)
include_directories(${EASYLOGGING_INCLUDE_DIR})