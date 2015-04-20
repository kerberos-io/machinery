message( "External project - Boost" )

ExternalProject_Add(boost
  GIT_REPOSITORY ${git_protocol}://github.com/boostorg/boost
  SOURCE_DIR boost
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ./bootstrap.sh
  BUILD_COMMAND ./b2
  INSTALL_DIR thirdparty 
)