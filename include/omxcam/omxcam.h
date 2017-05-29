#ifndef OMXCAM_H
#define OMXCAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <IL/OMX_Broadcom.h>

#include "omxcam_version.h"

#if __GNUC__ >= 4
# define OMXCAM_EXTERN __attribute__ ((visibility ("default")))
#else
# define OMXCAM_EXTERN //Empty
#endif

//Error definitions, expand if necessary
#define OMXCAM_ERRNO_MAP(X)                                                    \
  X (0, ERROR_NONE, "success")                                                 \
  X (1, ERROR_CAMERA_MODULE, "camera module is not ready to be used")          \
  X (2, ERROR_DRIVERS, "cannot load the camera drivers")                       \
  X (3, ERROR_INIT, "initialization error")                                    \
  X (4, ERROR_INIT_CAMERA, "cannot initialize the 'camera' component")         \
  X (5, ERROR_INIT_IMAGE_ENCODER, "cannot initialize the 'image_encode' "      \
      "component")                                                             \
  X (6, ERROR_INIT_VIDEO_ENCODER, "cannot initialize the 'video_encode' "      \
      "component")                                                             \
  X (7, ERROR_INIT_NULL_SINK, "cannot initialize the 'null_sink' component")   \
  X (8, ERROR_DEINIT, "deinitialization error")                                \
  X (9, ERROR_DEINIT_CAMERA, "cannot deinitialize the 'camera' component")     \
  X (10, ERROR_DEINIT_IMAGE_ENCODER, "cannot deinitialize the 'image_encode' " \
      "component")                                                             \
  X (11, ERROR_DEINIT_VIDEO_ENCODER, "cannot deinitialize the 'video_encode' " \
      "component")                                                             \
  X (12, ERROR_DEINIT_NULL_SINK, "cannot deinitialize the 'null_sink' "        \
      "component")                                                             \
  X (13, ERROR_CAPTURE, "error while capturing")                               \
  X (14, ERROR_CAMERA_RUNNING, "camera is already running")                    \
  X (15, ERROR_CAMERA_NOT_RUNNING, "camera is not running")                    \
  X (16, ERROR_CAMERA_STOPPING, "camera is already being stopped")             \
  X (17, ERROR_CAMERA_UPDATE, "camera is not ready to be updated")             \
  X (18, ERROR_BAD_PARAMETER, "incorrect parameter value")                     \
  X (19, ERROR_VIDEO_ONLY, "action can be executed only in video mode")        \
  X (20, ERROR_STILL, "still error")                                           \
  X (21, ERROR_VIDEO, "video error")                                           \
  X (22, ERROR_JPEG, "error configuring jpeg encoder")                         \
  X (23, ERROR_H264, "error configuring h264 encoder")                         \
  X (24, ERROR_LOADED, "cannot transition to the Loaded state")                \
  X (25, ERROR_IDLE, "cannot transition to the Idle state")                    \
  X (26, ERROR_EXECUTING, "cannot transition to the Executing state")          \
  X (27, ERROR_FORMAT, "invalid encoding format")                              \
  X (28, ERROR_SLEEP, "cannot sleep the thread")                               \
  X (29, ERROR_WAKE, "cannot wake the thread")                                 \
  X (30, ERROR_LOCK, "cannot lock the thread")                                 \
  X (31, ERROR_UNLOCK, "cannot unlock the thread")                             \
  X (32, ERROR_NO_PTHREAD, "capture started in 'no pthread' mode")             \
  X (33, ERROR_NOT_NO_PTHREAD, "capture started not in 'no pthread' mode")

#define OMXCAM_ISO_MAP_LENGTH 10
#define OMXCAM_ISO_MAP(X)                                                      \
  X (ISO_AUTO, 0)                                                              \
  X (ISO_100, 100)                                                             \
  X (ISO_160, 160)                                                             \
  X (ISO_200, 200)                                                             \
  X (ISO_250, 250)                                                             \
  X (ISO_320, 320)                                                             \
  X (ISO_400, 400)                                                             \
  X (ISO_500, 500)                                                             \
  X (ISO_640, 640)                                                             \
  X (ISO_800, 800)

#define OMXCAM_EXPOSURE_MAP_LENGTH 15
#define OMXCAM_EXPOSURE_MAP(X)                                                 \
  X (EXPOSURE_OFF, OMX_ExposureControlOff)                                     \
  X (EXPOSURE_AUTO, OMX_ExposureControlAuto)                                   \
  X (EXPOSURE_NIGHT, OMX_ExposureControlNight)                                 \
  X (EXPOSURE_BLACK_LIGHT, OMX_ExposureControlBackLight)                       \
  X (EXPOSURE_SPOTLIGHT, OMX_ExposureControlSpotLight)                         \
  X (EXPOSURE_SPORTS, OMX_ExposureControlSports)                               \
  X (EXPOSURE_SNOW, OMX_ExposureControlSnow)                                   \
  X (EXPOSURE_BEACH, OMX_ExposureControlBeach)                                 \
  X (EXPOSURE_LARGE_APERTURE, OMX_ExposureControlLargeAperture)                \
  X (EXPOSURE_SMALL_APERTURE, OMX_ExposureControlSmallAperture)                \
  X (EXPOSURE_VERY_LONG, OMX_ExposureControlVeryLong)                          \
  X (EXPOSURE_FIXED_FPS, OMX_ExposureControlFixedFps)                          \
  X (EXPOSURE_NIGHT_WITH_PREVIEW, OMX_ExposureControlNightWithPreview)         \
  X (EXPOSURE_ANTISHAKE, OMX_ExposureControlAntishake)                         \
  X (EXPOSURE_FIREWORKS, OMX_ExposureControlFireworks)

#define OMXCAM_MIRROR_MAP_LENGTH 4
#define OMXCAM_MIRROR_MAP(X)                                                   \
  X (MIRROR_NONE, OMX_MirrorNone)                                              \
  X (MIRROR_VERTICAL, OMX_MirrorVertical)                                      \
  X (MIRROR_HORIZONTAL, OMX_MirrorHorizontal)                                  \
  X (MIRROR_BOTH, OMX_MirrorBoth)

#define OMXCAM_ROTATION_MAP_LENGTH 4
#define OMXCAM_ROTATION_MAP(X)                                                 \
  X (ROTATION_NONE, 0)                                                         \
  X (ROTATION_90, 90)                                                          \
  X (ROTATION_180, 180)                                                        \
  X (ROTATION_270, 270)

#define OMXCAM_METERING_MAP_LENGTH 4
#define OMXCAM_METERING_MAP(X)                                                 \
  X (METERING_AVERAGE, OMX_MeteringModeAverage)                                \
  X (METERING_SPOT, OMX_MeteringModeSpot)                                      \
  X (METERING_MATRIX, OMX_MeteringModeMatrix)                                  \
  X (METERING_BACKLIT, OMX_MeteringModeBacklit)

#define OMXCAM_WHITE_BALANCE_MAP_LENGTH 10
#define OMXCAM_WHITE_BALANCE_MAP(X)                                            \
  X (WHITE_BALANCE_OFF, OMX_WhiteBalControlOff)                                \
  X (WHITE_BALANCE_AUTO, OMX_WhiteBalControlAuto)                              \
  X (WHITE_BALANCE_SUNLIGHT, OMX_WhiteBalControlSunLight)                      \
  X (WHITE_BALANCE_CLOUDY, OMX_WhiteBalControlCloudy)                          \
  X (WHITE_BALANCE_SHADE, OMX_WhiteBalControlShade)                            \
  X (WHITE_BALANCE_TUNGSTEN, OMX_WhiteBalControlTungsten)                      \
  X (WHITE_BALANCE_FLUORESCENT, OMX_WhiteBalControlFluorescent)                \
  X (WHITE_BALANCE_INCANDESCENT, OMX_WhiteBalControlIncandescent)              \
  X (WHITE_BALANCE_FLASH, OMX_WhiteBalControlFlash)                            \
  X (WHITE_BALANCE_HORIZON, OMX_WhiteBalControlHorizon)

#define OMXCAM_IMAGE_FILTER_MAP_LENGTH 18
#define OMXCAM_IMAGE_FILTER_MAP(X)                                             \
  X (IMAGE_FILTER_NONE, OMX_ImageFilterNone)                                   \
  X (IMAGE_FILTER_EMBOSS, OMX_ImageFilterEmboss)                               \
  X (IMAGE_FILTER_NEGATIVE, OMX_ImageFilterNegative)                           \
  X (IMAGE_FILTER_SKETCH, OMX_ImageFilterSketch)                               \
  X (IMAGE_FILTER_OILPAINT, OMX_ImageFilterOilPaint)                           \
  X (IMAGE_FILTER_HATCH, OMX_ImageFilterHatch)                                 \
  X (IMAGE_FILTER_GPEN, OMX_ImageFilterGpen)                                   \
  X (IMAGE_FILTER_SOLARIZE, OMX_ImageFilterSolarize)                           \
  X (IMAGE_FILTER_WATERCOLOR, OMX_ImageFilterWatercolor)                       \
  X (IMAGE_FILTER_PASTEL, OMX_ImageFilterPastel)                               \
  X (IMAGE_FILTER_FILM, OMX_ImageFilterFilm)                                   \
  X (IMAGE_FILTER_BLUR, OMX_ImageFilterBlur)                                   \
  X (IMAGE_FILTER_COLOUR_SWAP, OMX_ImageFilterColourSwap)                      \
  X (IMAGE_FILTER_WASHED_OUT, OMX_ImageFilterWashedOut)                        \
  X (IMAGE_FILTER_COLOUR_POINT, OMX_ImageFilterColourPoint)                    \
  X (IMAGE_FILTER_POSTERISE, OMX_ImageFilterPosterise)                         \
  X (IMAGE_FILTER_COLOUR_BALANCE, OMX_ImageFilterColourBalance)                \
  X (IMAGE_FILTER_CARTOON, OMX_ImageFilterCartoon)

#define OMXCAM_DRC_MAP_LENGTH 4
#define OMXCAM_DRC_MAP(X)                                                      \
  X (DRC_OFF, OMX_DynRangeExpOff)                                              \
  X (DRC_LOW, OMX_DynRangeExpLow)                                              \
  X (DRC_MEDIUM, OMX_DynRangeExpMedium)                                        \
  X (DRC_HIGH, OMX_DynRangeExpHigh)

#define OMXCAM_H264_AVC_PROFILE_MAP_LENGTH 3
#define OMXCAM_H264_AVC_PROFILE_MAP(X)                                         \
  X (H264_AVC_PROFILE_BASELINE, OMX_VIDEO_AVCProfileBaseline)                  \
  X (H264_AVC_PROFILE_MAIN, OMX_VIDEO_AVCProfileMain)                          \
  X (H264_AVC_PROFILE_HIGH, OMX_VIDEO_AVCProfileHigh)

#define OMXCAM_SHUTTER_SPEED_AUTO 0
#define OMXCAM_JPEG_THUMBNAIL_WIDTH_AUTO 0
#define OMXCAM_JPEG_THUMBNAIL_HEIGHT_AUTO 0
#define OMXCAM_H264_IDR_PERIOD_OFF 0
#define OMXCAM_H264_QP_OFF 0

//Handy way to sleep forever while recording a video
#define OMXCAM_CAPTURE_FOREVER 0

typedef enum {
  OMXCAM_FALSE,
  OMXCAM_TRUE
} omxcam_bool;

typedef enum {
  OMXCAM_FORMAT_RGB888,
  OMXCAM_FORMAT_RGBA8888,
  OMXCAM_FORMAT_YUV420,
  OMXCAM_FORMAT_JPEG,
  OMXCAM_FORMAT_H264
} omxcam_format;

#define OMXCAM_ENUM_FN(name, value)                                            \
  OMXCAM_ ## name = value,

typedef enum {
  OMXCAM_ISO_MAP (OMXCAM_ENUM_FN)
} omxcam_iso;

typedef enum {
  OMXCAM_EXPOSURE_MAP (OMXCAM_ENUM_FN)
} omxcam_exposure;

typedef enum {
  OMXCAM_MIRROR_MAP (OMXCAM_ENUM_FN)
} omxcam_mirror;

typedef enum {
  OMXCAM_ROTATION_MAP (OMXCAM_ENUM_FN)
} omxcam_rotation;

typedef enum {
  OMXCAM_METERING_MAP (OMXCAM_ENUM_FN)
} omxcam_metering;

typedef enum {
  OMXCAM_WHITE_BALANCE_MAP (OMXCAM_ENUM_FN)
} omxcam_white_balance;

typedef enum {
  OMXCAM_IMAGE_FILTER_MAP (OMXCAM_ENUM_FN)
} omxcam_image_filter;

typedef enum {
  OMXCAM_DRC_MAP (OMXCAM_ENUM_FN)
} omxcam_drc;

typedef enum {
  OMXCAM_H264_AVC_PROFILE_MAP (OMXCAM_ENUM_FN)
} omxcam_avc_profile;

#undef OMXCAM_ENUM_FN

#define OMXCAM_ENUM_FN(errno, name, _)                                         \
  OMXCAM_ ## name = errno,

typedef enum {
  OMXCAM_ERRNO_MAP (OMXCAM_ENUM_FN)
} omxcam_errno;

#undef OMXCAM_ENUM_FN

typedef struct {
  uint8_t* data;
  uint32_t length;
} omxcam_buffer_t;

typedef struct {
  omxcam_bool enabled;
  uint32_t u;
  uint32_t v;
} omxcam_color_effects_t;

typedef struct {
  omxcam_white_balance mode;
  uint32_t red_gain;
  uint32_t blue_gain;
} omxcam_white_balance_t;

typedef struct {
  uint32_t top;
  uint32_t left;
  uint32_t width;
  uint32_t height;
} omxcam_roi_t;

typedef struct {
  uint32_t width;
  uint32_t height;
  int32_t sharpness;
  int32_t contrast;
  uint32_t brightness;
  int32_t saturation;
  uint32_t shutter_speed;
  omxcam_iso iso;
  omxcam_exposure exposure;
  int32_t exposure_compensation;
  omxcam_mirror mirror;
  omxcam_rotation rotation;
  omxcam_color_effects_t color_effects;
  omxcam_bool color_denoise;
  omxcam_metering metering;
  omxcam_white_balance_t white_balance;
  omxcam_image_filter image_filter;
  omxcam_roi_t roi;
  omxcam_drc drc;
  //Used only in video mode
  uint32_t framerate;
  //Used only in video mode
  omxcam_bool frame_stabilisation;
} omxcam_camera_settings_t;

typedef struct {
  uint32_t offset_y;
  uint32_t length_y;
  uint32_t offset_u;
  uint32_t length_u;
  uint32_t offset_v;
  uint32_t length_v;
} omxcam_yuv_planes_t;

typedef struct {
  char* key;
  char* value;
} omxcam_exif_tag_t;

typedef struct {
  omxcam_bool enabled;
  omxcam_exif_tag_t* tags;
  uint32_t valid_tags;
} omxcam_exif_t;

typedef struct {
  omxcam_bool enabled;
  uint32_t width;
  uint32_t height;
  omxcam_bool preview;
} omxcam_thumbnail_t;

typedef struct {
  uint32_t quality;
  omxcam_exif_t exif;
  omxcam_bool ijg;
  omxcam_thumbnail_t thumbnail;
  omxcam_bool raw_bayer;
} omxcam_jpeg_settings_t;

typedef struct {
  omxcam_bool enabled;
  uint32_t loss_rate;
} omxcam_eede_t;

typedef struct {
  omxcam_bool enabled;
  uint32_t i;
  uint32_t p;
} omxcam_quantization_t;

typedef struct {
  uint32_t bitrate;
  uint32_t idr_period;
  omxcam_bool sei;
  omxcam_eede_t eede;
  omxcam_quantization_t qp;
  omxcam_avc_profile profile;
  omxcam_bool inline_headers;
  omxcam_bool inline_motion_vectors;
} omxcam_h264_settings_t;

#define OMXCAM_COMMON_SETTINGS                                                 \
  omxcam_camera_settings_t camera;                                             \
  omxcam_format format;                                                        \
  uint32_t camera_id;                                                          \
  void (*on_ready)();                                                          \
  void (*on_data)(omxcam_buffer_t buffer);                                     \
  void (*on_motion)(omxcam_buffer_t buffer);                                   \
  void (*on_stop)();

typedef struct {
  OMXCAM_COMMON_SETTINGS
  omxcam_jpeg_settings_t jpeg;
} omxcam_still_settings_t;

typedef struct {
  OMXCAM_COMMON_SETTINGS
  omxcam_h264_settings_t h264;
} omxcam_video_settings_t;

#undef OMXCAM_COMMON_SETTINGS

/*
 * Returns the string name of the given error. Returns NULL if the error is not
 * valid.
 */
OMXCAM_EXTERN const char* omxcam_error_name (omxcam_errno error);

/*
 * Returns the string message of the given error.
 */
OMXCAM_EXTERN const char* omxcam_strerror (omxcam_errno error);

/*
 * Returns the last error, if any.
 */
OMXCAM_EXTERN omxcam_errno omxcam_last_error ();

/*
 * Prints to stderr the last error in this way:
 *
 * omxcam: <error_name>: <strerror>\n
 */
OMXCAM_EXTERN void omxcam_perror ();

/*
 * Returns the library version packed into a single integer. 8 bits are used for
 * each component, with the patch number stored in the 8 least significant
 * bits, e.g. version 1.2.3 returns 0x010203.
 */
OMXCAM_EXTERN uint32_t omxcam_version ();

/*
* Returns the library version number as a string, e.g. 1.2.3
*/
OMXCAM_EXTERN const char* omxcam_version_string ();

/*
 * Rounds up a number given a divisor. For example, omxcam_round(1944, 16)
 * returns 1952. It is mainly used to align values according to the OpenMAX IL
 * and the Raspberry Pi camera board.
 */
OMXCAM_EXTERN uint32_t omxcam_round (uint32_t value, uint32_t divisor);

/*
 * Given the width and height of a frame, returns the offset and length of each
 * of the yuv planes.
 * 
 * For example, a 2592x1944 yuv frame is organized as follows:
 *
 * omxcam_yuv_planes_t yuv;
 * omxcam_yuv_planes (2592, 1944, &yuv);
 * printf (
 *   "y: %d %d\n"
 *   "u: %d %d\n"
 *   "v: %d %d\n",
 *   yuv.offset_y, yuv.length_y,
 *   yuv.offset_u, yuv.length_u,
 *   yuv.offset_v, yuv.length_v
 * );
 *
 * y: 0 5059584
 * u: 5059584 1264896
 * v: 6324480 1264896
 */
OMXCAM_EXTERN void omxcam_yuv_planes (
    uint32_t width,
    uint32_t height,
    omxcam_yuv_planes_t* planes);

/*
 * Same as 'omxcam_yuv_planes()' but used to calculate the offset and length of
 * the planes of a payload buffer.
 */
OMXCAM_EXTERN void omxcam_yuv_planes_slice (
    uint32_t width,
    omxcam_yuv_planes_t* planes);

/*
 * Sets the default settings for the image capture.
 */
OMXCAM_EXTERN void omxcam_still_init (omxcam_still_settings_t* settings);

/*
 * Starts the image capture with the given settings.
 */
OMXCAM_EXTERN int omxcam_still_start (omxcam_still_settings_t* settings);

/*
 * Stops the image capture and unblocks the current thread. It is safe to use
 * from anywhere in your code. You can call it from inside the 'on_data'
 * callback or from another thread.
 */
OMXCAM_EXTERN int omxcam_still_stop ();

/*
 * Sets the default settings for the video capture.
 */
OMXCAM_EXTERN void omxcam_video_init (omxcam_video_settings_t* settings);

/*
 * Starts the video capture with the given settings. While the video is being
 * streamed, the current thread is blocked. If you want to sleep the thread
 * forever, use the macro OMXCAM_CAPTURE_FOREVER:
 * 
 * omxcam_video_start(settings, OMXCAM_CAPTURE_FOREVER);
 */
OMXCAM_EXTERN int omxcam_video_start (
    omxcam_video_settings_t* settings,
    uint32_t ms);

/*
 * Stops the video capture and unblocks the current thread. It is safe to use
 * from anywhere in your code. You can call it from inside the 'on_data'
 * callback or from another thread.
 */
OMXCAM_EXTERN int omxcam_video_stop ();

/*
 * Replaces the video buffer callback. Can be only executed when the camera is
 * running.
 */
OMXCAM_EXTERN int omxcam_video_update_on_data (
    void (*on_data)(omxcam_buffer_t buffer));

/*
 * Updates the camera settings. Can be only executed while the camera is running
 * and it's in video mode.
 */
OMXCAM_EXTERN int omxcam_video_update_sharpness (int32_t sharpness);
OMXCAM_EXTERN int omxcam_video_update_contrast (int32_t contrast);
OMXCAM_EXTERN int omxcam_video_update_brightness (uint32_t brightness);
OMXCAM_EXTERN int omxcam_video_update_saturation (int32_t saturation);
OMXCAM_EXTERN int omxcam_video_update_iso (omxcam_iso iso);
OMXCAM_EXTERN int omxcam_video_update_exposure (omxcam_exposure exposure);
OMXCAM_EXTERN int omxcam_video_update_exposure_compensation (
    int32_t exposure_compensation);
OMXCAM_EXTERN int omxcam_video_update_mirror (omxcam_mirror mirror);
OMXCAM_EXTERN int omxcam_video_update_rotation (omxcam_rotation rotation);
OMXCAM_EXTERN int omxcam_video_update_color_effects (
    omxcam_color_effects_t* color_effects);
OMXCAM_EXTERN int omxcam_video_update_metering (omxcam_metering metering);
OMXCAM_EXTERN int omxcam_video_update_white_balance (
    omxcam_white_balance_t* white_balance);
OMXCAM_EXTERN int omxcam_video_update_image_filter (
    omxcam_image_filter image_filter);
OMXCAM_EXTERN int omxcam_video_update_roi (omxcam_roi_t* roi);
OMXCAM_EXTERN int omxcam_video_update_frame_stabilisation (
    omxcam_bool frame_stabilisation);

/*
 * Starts the video capture in "no pthread" mode. After this call the video
 * data is ready to be read.
 */
OMXCAM_EXTERN int omxcam_video_start_npt (omxcam_video_settings_t* settings);

/*
 * Stops the video capture in "no pthread" mode.
 */
OMXCAM_EXTERN int omxcam_video_stop_npt ();

/*
 * Fills a buffer with video data. This is a blocking function, that is, the
 * current thread is blocked until the buffer is filled. If an error occurs, the
 * video is stopped automatically.
 *
 * The "no pthread" functions don't have any multithread synchronization
 * mechanism, just the minimum required synchronization for communicating with
 * OpenMAX IL.
 *
 * They are useful when the client needs to request the buffers instead of
 * providing a callback to the camera to be called in a future, "give me data
 * and I'll wait for it" instead of "I give you a callback and I'll be doing
 * other things". The "no pthread" functions are useful when some kind of
 * inter-thread communication is required, otherwise, with the second approach
 * some kind of lock-free or blocking queue would be required (consumer-producer
 * problem). If the camera needs to be controlled by an asynchronous framework,
 * e.g.: libuv, these functions are very useful.
 */
int omxcam_video_read_npt (
    omxcam_buffer_t* buffer,
    omxcam_bool* is_motion_vector);

#ifdef __cplusplus
}
#endif

#endif