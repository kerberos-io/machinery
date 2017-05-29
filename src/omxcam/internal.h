#ifndef OMXCAM_INTERNAL_H
#define OMXCAM_INTERNAL_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include <bcm_host.h>
#include <interface/vmcs_host/vc_vchi_gencmd.h>

#define omxcam__omx_struct_init(x)                                             \
  memset (&(x), 0, sizeof (x));                                                \
  (x).nSize = sizeof (x);                                                      \
  (x).nVersion.nVersion = OMX_VERSION;                                         \
  (x).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;                            \
  (x).nVersion.s.nVersionMinor = OMX_VERSION_MINOR;                            \
  (x).nVersion.s.nRevision = OMX_VERSION_REVISION;                             \
  (x).nVersion.s.nStep = OMX_VERSION_STEP;

#define OMXCAM_CAMERA_NAME "OMX.broadcom.camera"
#define OMXCAM_IMAGE_ENCODE_NAME "OMX.broadcom.image_encode"
#define OMXCAM_VIDEO_ENCODE_NAME "OMX.broadcom.video_encode"
#define OMXCAM_NULL_SINK_NAME "OMX.broadcom.null_sink"

#define OMXCAM_MIN_GPU_MEM 128 //MB
#define OMXCAM_VIDEO_MAX_WIDTH 1920
#define OMXCAM_VIDEO_MAX_HEIGHT 1080
#define OMXCAM_STILL_MAX_WIDTH 2592
#define OMXCAM_STILL_MAX_HEIGHT 1944
#define OMXCAM_MIN_WIDTH 16
#define OMXCAM_MIN_HEIGHT 16

#ifdef OMXCAM_DEBUG
#define omxcam__error(message, ...)                                            \
  omxcam__error_(message, __func__, __FILE__, __LINE__, ## __VA_ARGS__)
#else
#define omxcam__error(message, ...) //Empty
#endif

#define OMXCAM_STR(x) #x
#define OMXCAM_STR_VALUE(x) OMXCAM_STR(x)

/*
 * Enumeration with a mapping between all the OpenMAX IL events and a unique
 * number. The id allows them to be bitwise-or'ed, e.g.
 * OMXCAM_EVENT_ERROR | OMXCAM_EVENT_PORT_ENABLE == 0x3
 */
typedef enum {
  OMXCAM_EVENT_ERROR = 0x1,
  OMXCAM_EVENT_PORT_ENABLE = 0x2,
  OMXCAM_EVENT_PORT_DISABLE = 0x4,
  OMXCAM_EVENT_STATE_SET = 0x8,
  OMXCAM_EVENT_FLUSH = 0x10,
  OMXCAM_EVENT_MARK_BUFFER = 0x20,
  OMXCAM_EVENT_MARK = 0x40,
  OMXCAM_EVENT_PORT_SETTINGS_CHANGED = 0x80,
  OMXCAM_EVENT_PARAM_OR_CONFIG_CHANGED = 0x100,
  OMXCAM_EVENT_BUFFER_FLAG = 0x200,
  OMXCAM_EVENT_RESOURCES_ACQUIRED = 0x400,
  OMXCAM_EVENT_DYNAMIC_RESOURCES_AVAILABLE = 0x800,
  OMXCAM_EVENT_FILL_BUFFER_DONE = 0x1000
} omxcam__event;

typedef enum {
  OMXCAM_STATE_LOADED = OMX_StateLoaded,
  OMXCAM_STATE_IDLE = OMX_StateIdle,
  OMXCAM_STATE_EXECUTING = OMX_StateExecuting,
  OMXCAM_STATE_PAUSE = OMX_StatePause,
  OMXCAM_STATE_WAIT_FOR_RESOURCES = OMX_StateWaitForResources,
  OMXCAM_STATE_INVALID = OMX_StateInvalid
} omxcam__state;

/*
 * Component's event flags.
 */
typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  uint32_t flags;
  OMX_ERRORTYPE omx_error;
  int fn_error;
} omxcam__event_t;

/*
 * Wrapper for an OpenMAX IL component.
 */
typedef struct {
  OMX_HANDLETYPE handle;
  omxcam__event_t event;
  OMX_STRING name;
} omxcam__component_t;

/*
 * Program's global context.
 */
typedef struct {
  omxcam__component_t camera;
  omxcam__component_t image_encode;
  omxcam__component_t video_encode;
  omxcam__component_t null_sink;
  OMX_BUFFERHEADERTYPE* output_buffer;
  void (*on_stop)();
  int video;
  int inline_motion_vectors;
  int no_pthread;
  int use_encoder;
  struct {
    int running;
    int joined;
    int stopping;
    int ready;
  } state;
} omxcam__context_t;

//Context's global variable
omxcam__context_t omxcam__ctx;

/*
 * Returns 'true' if OMXCAM_TRUE, 'false' if OMXCAM_FALSE.
 */
const char* omxcam__strbool (omxcam_bool value);

/*
 * Prints an error message to the stdout along with the file, line and function
 * name from where this function is called. It is printed if the cflag
 * OMXCAM_DEBUG is enabled. Use the omxcam__error() macro instead.
 */
void omxcam__error_ (
    const char* fmt,
    const char* fn,
    const char* file,
    int line,
    ...);

/*
 * Sets the last error.
 */
void omxcam__set_last_error (omxcam_errno error);

/*
 * OpenMAX IL event handlers.
 */
OMX_ERRORTYPE event_handler (
    OMX_HANDLETYPE comp,
    OMX_PTR app_data,
    OMX_EVENTTYPE event,
    OMX_U32 data1,
    OMX_U32 data2,
    OMX_PTR event_data);
OMX_ERRORTYPE fill_buffer_done (
    OMX_HANDLETYPE comp,
    OMX_PTR app_data,
    OMX_BUFFERHEADERTYPE* buffer);

/*
 * OpenMAX IL miscellaneous dump functions.
 */
const char* omxcam__dump_OMX_COLOR_FORMATTYPE (OMX_COLOR_FORMATTYPE type);
const char* omxcam__dump_OMX_OTHER_FORMATTYPE (OMX_OTHER_FORMATTYPE type);
const char* omxcam__dump_OMX_AUDIO_CODINGTYPE (OMX_AUDIO_CODINGTYPE type);
const char* omxcam__dump_OMX_VIDEO_CODINGTYPE (OMX_VIDEO_CODINGTYPE type);
const char* omxcam__dump_OMX_IMAGE_CODINGTYPE (OMX_IMAGE_CODINGTYPE type);
const char* omxcam__dump_OMX_STATETYPE (OMX_STATETYPE type);
const char* omxcam__dump_OMX_ERRORTYPE (OMX_ERRORTYPE type);
const char* omxcam__dump_OMX_EVENTTYPE (OMX_EVENTTYPE type);
const char* omxcam__dump_OMX_INDEXTYPE (OMX_INDEXTYPE type);
void omxcam__dump_OMX_PARAM_PORTDEFINITIONTYPE (
    OMX_PARAM_PORTDEFINITIONTYPE* type);
void omxcam__dump_OMX_IMAGE_PARAM_PORTFORMATTYPE (
    OMX_IMAGE_PARAM_PORTFORMATTYPE* type);
void omxcam__dump_OMX_BUFFERHEADERTYPE (OMX_BUFFERHEADERTYPE* type);

/*
 * Prints a debug message to the stdout. It is printed if the cflag OMXCAM_DEBUG
 * is enabled. 
 */
void omxcam__trace (const char* fmt, ...);

/*
 * Sets an error originated from the EventHandler.
 */
void omxcam__event_error (omxcam__component_t* component);

/*
 * Creates the event flags handler for the component.
 */
int omxcam__event_create (omxcam__component_t* component);

/*
 * Destroys the event flags handler for the component.
 */
int omxcam__event_destroy (omxcam__component_t* component);

/*
 * Sets some events.
 * 
 * Unlocks the current thread if there are events waiting to the given events.
 */
int omxcam__event_wake (
    omxcam__component_t* component,
    omxcam__event event,
    OMX_ERRORTYPE omx_error);
    
/*
 * Retrieve some events.
 *
 * Waits until the specified events have been set. For example, if
 * OMXCAM_EVENT_BUFFER_FLAG | OMXCAM_EVENT_MARK are passed, the thread will be
 * locked until it is woken up with the OMXCAM_EVENT_BUFFER_FLAG or
 * OMXCAM_EVENT_MARK events. When unlocked, the current events set are returned
 * in the 'current_events' pointer (null is allowed).
 *
 * OMXCAM_EVENT_ERROR is automatically handled.
 */
int omxcam__event_wait (
    omxcam__component_t* component,
    omxcam__event events,
    omxcam__event* current_events,
    OMX_ERRORTYPE* omx_error);

/*
 * Enables and disables the port of a component.
 */
int omxcam__component_port_enable (
    omxcam__component_t* component,
    uint32_t port);
int omxcam__component_port_disable (
    omxcam__component_t* component,
    uint32_t port);

/*
 * Performs cleanup tasks before exit. These tasks doesn't return any error.
 * The main usage is to call this function in the return of the function that
 * was called by the user. The return is always the 'code' parameter.
 * 
 * For example:
 *
 * int omxcam_video_start(...){
 *   ...
 *   return omxcam__exit (omxcam_video_stop ());
 * }
 */
int omxcam__exit (int code);

/*
 * Same as 'omxcam__exit()' but used with the 'no pthread' functions.
 */
int omxcam__exit_npt (int code);

/*
 * Initializes a component. All its ports are enabled and the OpenMAX IL event
 * handlers are configured, plus other things.
 */
int omxcam__component_init (omxcam__component_t* component);

/*
 * Deinitializes a component. All its ports are disabled, plus other things.
 */
int omxcam__component_deinit (omxcam__component_t* component);

/*
 * Changes the state of a component.
 */
int omxcam__component_change_state (
    omxcam__component_t* component,
    omxcam__state state);

/*
 * Allocates and frees an OpenMAX buffer for the given port.
 */
int omxcam__buffer_alloc (omxcam__component_t* component, uint32_t port);
int omxcam__buffer_free (omxcam__component_t* component, uint32_t port);

/*
 * Initializes and deinitializes OpenMAX IL. They must be the first and last
 * api calls.
 */
int omxcam__init ();
int omxcam__deinit ();

/*
 * Loads the camera drivers. After this call the red led is turned on and the
 * OpenMAX IL layer is ready to be configured.
 */
int omxcam__camera_load_drivers ();

/*
 * Checks if the camera is ready to be used. It checks the available gpu memory
 * and whether it is supported and detected.
 */
int omxcam__camera_check ();

/*
 * The camera needs to know which output port is going to be used to consume the
 * data. The capture port must be enabled just before the buffers are going to
 * be consumed, that is, when the state of the camera and all the enabled
 * components are OMX_StateExecuting.
 * 
 * video = 71
 * still = 72
 */
int omxcam__camera_capture_port_set (uint32_t port);
int omxcam__camera_capture_port_reset (uint32_t port);

/*
 * Sets the default settings for the camera.
 */
void omxcam__camera_init (
    omxcam_camera_settings_t* settings,
    uint32_t width,
    uint32_t height);

/*
 * Configures the OpenMAX IL camera component.
 */
int omxcam__camera_configure_omx (
    omxcam_camera_settings_t* settings,
    int video);

/*
 * Configures the camera settings.
 */
int omxcam__camera_set_sharpness (int32_t sharpness);
int omxcam__camera_set_contrast (int32_t contrast);
int omxcam__camera_set_brightness (uint32_t brightness);
int omxcam__camera_set_saturation (int32_t saturation);
int omxcam__camera_set_iso (omxcam_iso iso);
int omxcam__camera_set_exposure (omxcam_exposure exposure);
int omxcam__camera_set_exposure_compensation (int32_t exposure_compensation);
int omxcam__camera_set_mirror (omxcam_mirror mirror, int video);
int omxcam__camera_set_rotation (omxcam_rotation rotation, int video);
int omxcam__camera_set_color_effects (
    omxcam_color_effects_t* color_effects);
int omxcam__camera_set_color_denoise (omxcam_bool color_denoise);
int omxcam__camera_set_metering (omxcam_metering metering);
int omxcam__camera_set_white_balance (omxcam_white_balance_t* white_balance);
int omxcam__camera_set_image_filter (omxcam_image_filter image_filter);
int omxcam__camera_set_roi (omxcam_roi_t* roi);
int omxcam__camera_set_frame_stabilisation (omxcam_bool frame_stabilisation);

/*
 * Validates the settings.
 */
int omxcam__still_validate (omxcam_still_settings_t* settings);
int omxcam__video_validate (omxcam_video_settings_t* settings);
int omxcam__camera_validate (omxcam_camera_settings_t* settings, int video);
int omxcam__jpeg_validate (omxcam_jpeg_settings_t* settings);
int omxcam__h264_validate (omxcam_h264_settings_t* settings);

/*
 * Validates each camera setting. Returns 1 if it's valid, 0 otherwise.
 */
int omxcam__camera_is_valid_width (uint32_t width, int video);
int omxcam__camera_is_valid_height (uint32_t height, int video);
int omxcam__camera_is_valid_sharpness (int32_t sharpness);
int omxcam__camera_is_valid_contrast (int32_t contrast);
int omxcam__camera_is_valid_brightness (uint32_t brightness);
int omxcam__camera_is_valid_saturation (int32_t saturation);
int omxcam__camera_is_valid_iso (omxcam_iso iso);
int omxcam__camera_is_valid_exposure (omxcam_exposure exposure);
int omxcam__camera_is_valid_exposure_compensation (
    int32_t exposure_compensation);
int omxcam__camera_is_valid_mirror (omxcam_mirror mirror);
int omxcam__camera_is_valid_rotation (omxcam_rotation rotation);
int omxcam__camera_is_valid_color_effects (uint32_t color);
int omxcam__camera_is_valid_metering (omxcam_metering metering);
int omxcam__camera_is_valid_white_balance (omxcam_white_balance white_balance);
int omxcam__camera_is_valid_image_filter (omxcam_image_filter image_filter);
int omxcam__camera_is_valid_drc (omxcam_drc drc);
int omxcam__camera_is_valid_roi (uint32_t roi);
int omxcam__camera_is_valid_framerate (uint32_t framerate);

/*
 * Returns the string name of the given camera setting.
 */
const char* omxcam__camera_str_iso (omxcam_iso iso);
const char* omxcam__camera_str_exposure (omxcam_exposure exposure);
const char* omxcam__camera_str_mirror (omxcam_mirror mirror);
const char* omxcam__camera_str_rotation (omxcam_rotation rotation);
const char* omxcam__camera_str_metering (omxcam_metering metering);
const char* omxcam__camera_str_white_balance (
    omxcam_white_balance white_balance);
const char* omxcam__camera_str_image_filter (omxcam_image_filter image_filter);

/*
 * Sets the default settings for the jpeg encoder.
 */
void omxcam__jpeg_init (omxcam_jpeg_settings_t* settings);

/*
 * Adds an exif tag to the jpeg metadata.
 */
int omxcam__jpeg_add_tag (char* key, char* value);

/*
 * Configures the OpenMAX IL image_encode component with the jpeg settings.
 */
int omxcam__jpeg_configure_omx (omxcam_jpeg_settings_t* settings);

/*
 * Validates each jpeg setting. Returns 1 if it's valid, 0 otherwise.
 */
int omxcam__jpeg_is_valid_quality (uint32_t quality);
int omxcam__jpeg_is_valid_thumbnail (uint32_t dimension);

/*
 * Sets the default settings for the h264 encoder.
 */
void omxcam__h264_init (omxcam_h264_settings_t* settings);

/*
 * Configures the OpenMAX IL video_encode component with the h264 settings.
 */
int omxcam__h264_configure_omx (omxcam_h264_settings_t* settings);

/*
 * Returns the string name of the given h246 setting.
 */
const char* omxcam__h264_str_avc_profile (omxcam_avc_profile profile);

/*
 * Validates each h264 setting. Returns 1 if it's valid, 0 otherwise.
 */
int omxcam__h264_is_valid_bitrate (uint32_t bitrate);
int omxcam__h264_is_valid_eede_loss_rate (uint32_t loss_rate);
int omxcam__h264_is_valid_quantization (uint32_t qp);
int omxcam__h264_is_valid_avc_profile (omxcam_avc_profile profile);

#endif