message("External project: Googlemock")

ExternalProject_Add(googlemock
  SVN_REPOSITORY https://github.com/google/googlemock
  SOURCE_DIR googlemock
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=Release
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
    -Dgtest_force_shared_crt=OFF
  BUILD_COMMAND cd googlemock && make && make test
  INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty
  INSTALL_COMMAND ""
)

set(GOOGLEMOCK_INCLUDE_DIR ${CMAKE_BINARY_DIR}/googlemock/include/)
set(GOOGLEMOCK_LIBRARY_DIR ${CMAKE_BINARY_DIR}/)

include_directories(${GOOGLEMOCK_INCLUDE_DIR})
link_directories(${GOOGLEMOCK_LIBRARY_DIR})

set(GOOGLEMOCK_LIBRARIES "${GOOGLEMOCK_LIBRARY_DIR}googlemock/libgmock.a" "${GOOGLEMOCK_LIBRARY_DIR}googlemock/libgmock_main.a")
