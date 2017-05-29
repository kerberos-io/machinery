#include "omxcam.h"
#include "internal.h"

#define OMXCAM_VERSION                                                         \
  ((OMXCAM_VERSION_MAJOR << 16) |                                              \
  (OMXCAM_VERSION_MINOR << 8) |                                                \
  (OMXCAM_VERSION_PATCH))

#define OMXCAM_VERSION_STRING                                                  \
  OMXCAM_STR_VALUE(OMXCAM_VERSION_MAJOR) "."                                   \
  OMXCAM_STR_VALUE(OMXCAM_VERSION_MINOR) "."                                   \
  OMXCAM_STR_VALUE(OMXCAM_VERSION_PATCH)

uint32_t omxcam_version (){
  return OMXCAM_VERSION;
}

const char* omxcam_version_string (){
  return OMXCAM_VERSION_STRING;
}