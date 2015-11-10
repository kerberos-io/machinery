message("External project: Googletest")

ExternalProject_Add(googletest
  SVN_REPOSITORY https://github.com/google/googletest
  SOURCE_DIR googletest
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=Release
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
    -Dgtest_force_shared_crt=OFF
  BUILD_COMMAND cd googletest && make && make test
  INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty
  INSTALL_COMMAND ""
)

set(GOOGLETEST_INCLUDE_DIR ${CMAKE_BINARY_DIR}/googletest/include/)
set(GOOGLETEST_LIBRARY_DIR ${CMAKE_BINARY_DIR}/)

include_directories(${GOOGLETEST_INCLUDE_DIR})
link_directories(${GOOGLETEST_LIBRARY_DIR})

set(GOOGLETEST_LIBRARIES "${GOOGLETEST_LIBRARY_DIR}googletest/libgtest.a" "${GOOGLETEST_LIBRARY_DIR}googletest/libgtest_main.a")