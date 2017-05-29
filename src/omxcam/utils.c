#include "omxcam.h"
#include "internal.h"

uint32_t omxcam_round (uint32_t value, uint32_t multiplier){
  //Assumed that the rounding value is a power of 2
  return (value + multiplier - 1) & ~(multiplier - 1);
}

void omxcam_yuv_planes (
    uint32_t width,
    uint32_t height,
    omxcam_yuv_planes_t* planes){
  width = omxcam_round (width, 32);
  height = omxcam_round (height, 16);
  
  planes->offset_y = 0;
  planes->length_y = width*height;
  planes->offset_u = planes->length_y;
  //(width/2)*(height/2)
  planes->length_u = (width >> 1)*(height >> 1);
  planes->offset_v = planes->length_y + planes->length_u;
  planes->length_v = planes->length_u;
}

void omxcam_yuv_planes_slice (
    uint32_t width,
    omxcam_yuv_planes_t* planes){
  width = omxcam_round (width, 32);

  //slice height = 16 
  planes->offset_y = 0;
  planes->length_y = width << 4;
  planes->offset_u = planes->length_y;
  //(width/2)*(sliceHeight/2)
  planes->length_u = width << 2;
  planes->offset_v = planes->length_y + planes->length_u;
  planes->length_v = planes->length_u;
}

const char* omxcam__strbool (omxcam_bool value){
  return value ? "true" : "false";
}