message("External project: Restclient")

ExternalProject_Add(restclient
  GIT_REPOSITORY ${git_protocol}://github.com/mrtazz/restclient-cpp
  SOURCE_DIR restclient
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ./autogen.sh && ./configure
  BUILD_COMMAND make && ln -s .libs/librestclient-cpp.0.dylib ../thirdparty/lib/librestclient-cpp.0.dylib
  INSTALL_COMMAND ""
)

set(RESTCLIENT_INCLUDE_DIR ${CMAKE_BINARY_DIR}/restclient/include/)
set(RESTCLIENT_LIBRARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/lib/)

set(RESTCLIENT_LIBRARIES restclient-cpp)

include_directories(${RESTCLIENT_INCLUDE_DIR})
link_directories(${RESTCLIENT_LIBRARY_DIR})