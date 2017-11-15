message("External project: Mosquitto")

ExternalProject_Add(mosquitto
  GIT_REPOSITORY ${git_protocol}://github.com/eclipse/mosquitto.git
  GIT_TAG d3239920d789a16cb9c54c598f4fc7d5ef03f2e9
  SOURCE_DIR mosquitto
  BINARY_DIR mosquitto-build
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_GENERATOR ${gen}
  INSTALL_COMMAND mkdir -p ../thirdparty/lib/ && cp lib/cpp/libmosquittopp.a ../thirdparty/lib/
  CMAKE_ARGS
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/../../host/usr/share/buildroot/toolchainfile.cmake
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
    -DWITH_UUID=no
    -DWITH_WEBSOCKETS=no
    -DWITH_SRV=no
)

set(MOSQUITTO_INCLUDE_DIR ${CMAKE_BINARY_DIR}/mosquitto/lib/ ${CMAKE_BINARY_DIR}/mosquitto/lib/cpp/)
set(MOSQUITTO_LIBRARY_DIR  ${CMAKE_BINARY_DIR}/mosquitto-build/lib/cpp/)

set(MOSQUITTO_LIBRARIES  mosquittopp.a crypto ssl)

include_directories(${MOSQUITTO_INCLUDE_DIR})
