message("External project: Googletest")

ExternalProject_Add(googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG 673c975a963f356b19fea90cb57b69192253da2a
  SOURCE_DIR googletest
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=Release
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
    -Dgtest_force_shared_crt=OFF
  INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty
  INSTALL_COMMAND ""
)

set(GOOGLETEST_INCLUDE_DIR "${CMAKE_BINARY_DIR}/googletest/googletest/include/" "${CMAKE_BINARY_DIR}/googletest/googlemock/include/")
set(GOOGLETEST_LIBRARY_DIR ${CMAKE_BINARY_DIR}/googletest/)

include_directories(${GOOGLETEST_INCLUDE_DIR})
link_directories(${GOOGLETEST_LIBRARY_DIR})

set(GOOGLETEST_LIBRARIES "${GOOGLETEST_LIBRARY_DIR}googlemock/libgmock.a" "${GOOGLETEST_LIBRARY_DIR}googlemock/libgmock_main.a")
