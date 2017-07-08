message("External project: Restclient")

ExternalProject_Add(restclient
  GIT_REPOSITORY ${git_protocol}://github.com/mrtazz/restclient-cpp
  GIT_TAG 0.3.0
  SOURCE_DIR restclient
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ./autogen.sh && ./configure
  BUILD_COMMAND make && mkdir -p ../thirdparty/lib/ && cp .libs/librestclient-cpp.a ../thirdparty/lib/
  INSTALL_COMMAND ""
)

set(RESTCLIENT_INCLUDE_DIR ${CMAKE_BINARY_DIR}/restclient/include/)
set(RESTCLIENT_LIBRARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/lib/)

set(RESTCLIENT_LIBRARIES restclient-cpp curl)

include_directories(${RESTCLIENT_INCLUDE_DIR})
link_directories(${RESTCLIENT_LIBRARY_DIR})