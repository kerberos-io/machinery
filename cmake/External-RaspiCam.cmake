# -----------------------------------
# Userland, ARM used by raspicam

message("Bind project: Userland")

set(USERLAND_INCLUDE_DIR /opt/vc/include/)
set(USERLAND_LIBRARY_DIR /opt/vc/lib/)

include_directories(${USERLAND_INCLUDE_DIR})
link_directories(${USERLAND_LIBRARY_DIR})

# -----------------------------------
# Raspicam

message("External project: RaspiCam")

ExternalProject_Add(raspicamera
  GIT_REPOSITORY ${git_protocol}://github.com/cedricve/raspicam
  SOURCE_DIR raspicamera
  BINARY_DIR raspicamera-build
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
    -DBUILD_SHARED_LIBS=OFF
    -DBUILD_UTILS=OFF
)

set(RASPBERRYPI_INCLUDE_DIR ${CMAKE_BINARY_DIR}/thirdparty/include/)
set(RASPBERRYPI_LIBRARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/lib/)

set(RASPBERRYPI_LIBRARIES raspicam.a raspicam_cv.a mmal mmal_core mmal_util)

include_directories(${RASPBERRYPI_INCLUDE_DIR})
link_directories(${RASPBERRYPI_LIBRARY_DIR})