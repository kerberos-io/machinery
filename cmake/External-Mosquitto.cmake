message("External project: Mosquitto")

ExternalProject_Add(mosquitto
  GIT_REPOSITORY ${git_protocol}://github.com/eclipse/mosquitto.git
  GIT_TAG ff55499725e8ae5cf4ab6a441541f0a6d1fe30f1
  SOURCE_DIR mosquitto
  BINARY_DIR mosquitto-build
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_GENERATOR ${gen}
  INSTALL_COMMAND mkdir -p ../thirdparty/lib/ && cp lib/cpp/libmosquittopp.a ../thirdparty/lib/ && cp lib/libmosquitto.a ../thirdparty/lib/
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
    -DWITH_WEBSOCKETS=OFF
    -DWITH_SRV=OFF
    -DWITH_STATIC_LIBRARIES=ON
)

set(MOSQUITTO_INCLUDE_DIR ${CMAKE_BINARY_DIR}/mosquitto/lib/ ${CMAKE_BINARY_DIR}/mosquitto/lib/cpp/)
set(MOSQUITTO_LIBRARY_DIR  ${CMAKE_BINARY_DIR}/mosquitto-build/lib/ ${CMAKE_BINARY_DIR}/mosquitto-build/lib/cpp/)

set(MOSQUITTO_LIBRARIES mosquittopp.a mosquitto.a crypto ssl)

include_directories(${MOSQUITTO_INCLUDE_DIR})
