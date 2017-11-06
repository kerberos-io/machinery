message("External project: Mosquitto")

ExternalProject_Add(mosquitto 
  GIT_REPOSITORY ${git_protocol}://github.com/eclipse/mosquitto.git
  GIT_TAG master
  SOURCE_DIR mosquitto
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND cd lib && make WITH_UUID=no WITH_WEBSOCKETS=no WITH_SRV=no && mkdir -p ../../thirdparty/lib/ && ar rcT ../../thirdparty/lib/libmosquitto-cpp.a libmosquitto.a cpp/mosquittopp.o
  INSTALL_COMMAND ""
)

set(MOSQUITTO_INCLUDE_DIR ${CMAKE_BINARY_DIR}/mosquitto/lib/cpp/)
set(MOSQUITTO_LIBRARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/lib/)

set(MOSQUITTO_LIBRARIES  mosquitto-cpp)

include_directories(${MOSQUITTO_INCLUDE_DIR})
