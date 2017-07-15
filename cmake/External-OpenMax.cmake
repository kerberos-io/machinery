# -----------------------------------
# Userland, ARM used by raspicam

message("Bind project: OpenMAX")

set(USERLAND_INCLUDE_DIR /opt/vc/include/)
set(USERLAND_LIBRARY_DIR /opt/vc/lib/)

include_directories(${USERLAND_INCLUDE_DIR})
link_directories(${USERLAND_LIBRARY_DIR})

# -----------------------------------
# OpenMaxIL-cpp

message("External project: OpenMaxIL-cpp")

ExternalProject_Add(openmax
  GIT_REPOSITORY ${git_protocol}://github.com/dridri/OpenMaxIL-cpp
  SOURCE_DIR openmax
  BINARY_DIR openmax-build
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_GENERATOR ${gen}
  INSTALL_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
    -DBUILD_SHARED_LIBS=OFF   
)

set(OPENMAX_INCLUDE_DIR ${CMAKE_BINARY_DIR}/openmax/include/)
set(OPENMAX_LIBRARY_DIR ${CMAKE_BINARY_DIR}/openmax-build/)

set(OPENMAX_LIBRARIES -lopenmaxil -lbcm_host -lvcsm -lvcos -lvchiq_arm -lrt -lpthread -ldl "OpenMaxIL++")

include_directories(${OPENMAX_INCLUDE_DIR})
link_directories(${OPENMAX_LIBRARY_DIR})
