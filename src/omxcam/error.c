#include "omxcam.h"
#include "internal.h"

static omxcam_errno last_error = OMXCAM_ERROR_NONE;

void omxcam__error_ (
    const char* fmt,
    const char* fn,
    const char* file,
    int line,
    ...){
  char buffer[256];
  va_list args;
  va_start (args, line);
  vsprintf (buffer, fmt, args);
  va_end (args);
  
  omxcam__trace ("error: %s (function: '%s', file: '%s', line: %d)", buffer,
      fn, file, line);
}

#define OMXCAM_NAME_FN(_, name, __)                                            \
  case OMXCAM_ ## name: return "OMXCAM_" #name;

const char* omxcam_error_name (omxcam_errno error){
  switch (error){
    OMXCAM_ERRNO_MAP (OMXCAM_NAME_FN)
    default: return 0;
  }
}

#undef OMXCAM_NAME_FN

#define OMXCAM_STRERROR_FN(_, name, msg)                                       \
  case OMXCAM_ ## name: return msg;

const char* omxcam_strerror (omxcam_errno error){
  switch (error){
    OMXCAM_ERRNO_MAP (OMXCAM_STRERROR_FN)
    default: return "unknown omxcam error";
  }
}

#undef OMXCAM_STRERROR_FN

omxcam_errno omxcam_last_error (){
  return last_error;
}

void omxcam__set_last_error (omxcam_errno error){
  last_error = error;
}

void omxcam_perror (){
  fprintf (stderr, "omxcam: %s: %s\n", omxcam_error_name (last_error),
      omxcam_strerror (last_error));
}