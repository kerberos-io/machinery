message("External project: Userland")

ExternalProject_Add(userland
  GIT_REPOSITORY ${git_protocol}://github.com/raspberrypi/userland
  SOURCE_DIR userland
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_COMMAND ./buildme
)

set(USERLAND_INCLUDE_DIR /opt/vc/include/)
set(USERLAND_LIBRARY_DIR /opt/vc/lib/)

include_directories(${USERLAND_INCLUDE_DIR})
link_directories(${USERLAND_LIBRARY_DIR})