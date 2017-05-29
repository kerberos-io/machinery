#include "omxcam.h"
#include "internal.h"

int omxcam__camera_load_drivers (uint32_t camera_id){
  /*
  This is a specific behaviour of the Broadcom's Raspberry Pi OpenMAX IL
  implementation module because the OMX_SetConfig() and OMX_SetParameter() are
  blocking functions but the drivers are loaded asynchronously, that is, an
  event is fired to signal the completion. Basically, what you're saying is:
  
  "When the parameter with index OMX_IndexParamCameraDeviceNumber is set, load
  the camera drivers and emit an OMX_EventParamOrConfigChanged event"
  
  The red led of the camera will be turned on after this call.
  */
  
  omxcam__trace ("loading '%s' drivers", omxcam__ctx.camera.name);
  
  OMX_ERRORTYPE error;

  OMX_CONFIG_REQUESTCALLBACKTYPE req_st;
  omxcam__omx_struct_init (req_st);
  req_st.nPortIndex = OMX_ALL;
  req_st.nIndex = OMX_IndexParamCameraDeviceNumber;
  req_st.bEnable = OMX_TRUE;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigRequestCallback, &req_st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigRequestCallback: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  OMX_PARAM_U32TYPE dev_st;
  omxcam__omx_struct_init (dev_st);
  dev_st.nPortIndex = OMX_ALL;
  dev_st.nU32 = camera_id;
  if ((error = OMX_SetParameter (omxcam__ctx.camera.handle,
      OMX_IndexParamCameraDeviceNumber, &dev_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamCameraDeviceNumber: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return omxcam__event_wait (&omxcam__ctx.camera,
      OMXCAM_EVENT_PARAM_OR_CONFIG_CHANGED, 0, 0);
}

int omxcam__camera_check (){
  char buffer[32];
  int mem_gpu = 0;
  int supported = 0;
  int detected = 0;
  
  if (!vc_gencmd (buffer, sizeof (buffer), "get_mem gpu")){
    vc_gencmd_number_property (buffer, "gpu", &mem_gpu);
  }
  
  if (mem_gpu < OMXCAM_MIN_GPU_MEM){
    omxcam__error ("memory configured for the gpu is smaller than the minimum "
        "required: current %d, minimum %d", mem_gpu, OMXCAM_MIN_GPU_MEM);
    return -1;
  }
  
  if (!vc_gencmd (buffer, sizeof (buffer), "get_camera")){
    vc_gencmd_number_property (buffer, "supported", &supported);
    vc_gencmd_number_property (buffer, "detected", &detected);
  }
  
  if (!supported){
    omxcam__error ("camera is not enabled in this build");
    return -1;
  }
  
  if (!detected){
    omxcam__error ("camera is not detected");
    return -1;
  }
  
  return 0;
}

static int omxcam__config_capture_port (uint32_t port, OMX_BOOL set){
  OMX_ERRORTYPE error;
  
  OMX_CONFIG_PORTBOOLEANTYPE port_st;
  omxcam__omx_struct_init (port_st);
  port_st.nPortIndex = port;
  port_st.bEnabled = set;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigPortCapturing, &port_st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigPortCapturing: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__camera_capture_port_set (uint32_t port){
  omxcam__trace ("setting '%s' capture port", omxcam__ctx.camera.name);
  return omxcam__config_capture_port (port, OMX_TRUE);
}

int omxcam__camera_capture_port_reset (uint32_t port){
  omxcam__trace ("resetting '%s' capture port", omxcam__ctx.camera.name);
  return omxcam__config_capture_port (port, OMX_FALSE);
}

void omxcam__camera_init (
    omxcam_camera_settings_t* settings,
    uint32_t width,
    uint32_t height){
  settings->width = width;
  settings->height = height;
  settings->sharpness = 0;
  settings->contrast = 0;
  settings->brightness = 50;
  settings->saturation = 0;
  settings->shutter_speed = OMXCAM_SHUTTER_SPEED_AUTO;
  settings->iso = OMXCAM_ISO_AUTO;
  settings->exposure = OMXCAM_EXPOSURE_AUTO;
  settings->exposure_compensation = 0;
  settings->mirror = OMXCAM_MIRROR_NONE;
  settings->rotation = OMXCAM_ROTATION_NONE;
  settings->color_effects.enabled = OMXCAM_FALSE;
  settings->color_effects.u = 128;
  settings->color_effects.v = 128;
  settings->color_denoise = OMXCAM_TRUE;
  settings->metering = OMXCAM_METERING_AVERAGE;
  settings->white_balance.mode = OMXCAM_WHITE_BALANCE_AUTO;
  settings->white_balance.red_gain = 100;
  settings->white_balance.blue_gain = 100;
  settings->image_filter = OMXCAM_IMAGE_FILTER_NONE;
  settings->roi.top = 0;
  settings->roi.left = 0;
  settings->roi.width = 100;
  settings->roi.height = 100;
  settings->drc = OMXCAM_DRC_OFF;
  settings->framerate = 30;
  settings->frame_stabilisation = OMXCAM_FALSE;
}

int omxcam__camera_validate (omxcam_camera_settings_t* settings, int video){
  if (!omxcam__camera_is_valid_width (settings->width, video)){
    omxcam__error ("invalid 'camera.width' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_height (settings->height, video)){
    omxcam__error ("invalid 'camera.height' value");
    return -1;
  }
  if (settings->width < settings->height){
    omxcam__error ("'camera.width' must be >= than 'camera.height'");
    return -1;
  }
  if (!omxcam__camera_is_valid_sharpness (settings->sharpness)){
    omxcam__error ("invalid 'camera.sharpness' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_contrast (settings->contrast)){
    omxcam__error ("invalid 'camera.contrast' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_brightness (settings->brightness)){
    omxcam__error ("invalid 'camera.brightness' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_saturation (settings->saturation)){
    omxcam__error ("invalid 'camera.saturation' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_iso (settings->iso)){
    omxcam__error ("invalid 'camera.iso' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_exposure (settings->exposure)){
    omxcam__error ("invalid 'camera.exposure' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_exposure_compensation (
      settings->exposure_compensation)){
    omxcam__error ("invalid 'camera.exposure_compensation' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_mirror (settings->mirror)){
    omxcam__error ("invalid 'camera.mirror' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_rotation (settings->rotation)){
    omxcam__error ("invalid 'camera.rotation' value");
    return -1;
  }
  if (settings->color_effects.enabled){
    if (!omxcam__camera_is_valid_color_effects (settings->color_effects.u)){
      omxcam__error ("invalid 'camera.color_effects.u' value");
      return -1;
    }
    if (!omxcam__camera_is_valid_color_effects (settings->color_effects.v)){
      omxcam__error ("invalid 'camera.color_effects.v' value");
      return -1;
    }
  }
  if (!omxcam__camera_is_valid_metering (settings->metering)){
    omxcam__error ("invalid 'camera.metering' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_white_balance (settings->white_balance.mode)){
    omxcam__error ("invalid 'camera.white_balance.mode' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_image_filter (settings->image_filter)){
    omxcam__error ("invalid 'camera.image_filter' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_drc (settings->drc)){
    omxcam__error ("invalid 'camera.drc' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_roi (settings->roi.top)){
    omxcam__error ("invalid 'camera.roi.top' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_roi (settings->roi.left)){
    omxcam__error ("invalid 'camera.roi.left' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_roi (settings->roi.width)){
    omxcam__error ("invalid 'camera.roi.width' value");
    return -1;
  }
  if (!omxcam__camera_is_valid_roi (settings->roi.height)){
    omxcam__error ("invalid 'camera.roi.height' value");
    return -1;
  }
  if (video && !omxcam__camera_is_valid_framerate (settings->framerate)){
    omxcam__error ("invalid 'camera.framerate' value");
    return -1;
  }
  
  return 0;
}

int omxcam__camera_set_sharpness (int32_t sharpness){
  OMX_ERRORTYPE error;
  OMX_CONFIG_SHARPNESSTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.nSharpness = sharpness;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonSharpness, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonSharpness: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_contrast (int32_t contrast){
  OMX_ERRORTYPE error;
  OMX_CONFIG_CONTRASTTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.nContrast = contrast;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonContrast, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonContrast: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_brightness (uint32_t brightness){
  OMX_ERRORTYPE error;
  OMX_CONFIG_BRIGHTNESSTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.nBrightness = brightness;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonBrightness, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonBrightness: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_saturation (int32_t saturation){
  OMX_ERRORTYPE error;
  OMX_CONFIG_SATURATIONTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.nSaturation = saturation;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonSaturation, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonSaturation: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_iso (omxcam_iso iso){
  OMX_ERRORTYPE error;
  OMX_CONFIG_EXPOSUREVALUETYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  if ((error = OMX_GetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonExposureValue, &st))){
    omxcam__error ("OMX_GetConfig - OMX_IndexConfigCommonExposureValue: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  st.nSensitivity = iso;
  st.bAutoSensitivity = !iso;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonExposureValue, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonExposureValue: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_exposure (omxcam_exposure exposure){
  OMX_ERRORTYPE error;
  OMX_CONFIG_EXPOSURECONTROLTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.eExposureControl = exposure;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonExposure, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonExposure: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_exposure_compensation (int32_t exposure_compensation){
  OMX_ERRORTYPE error;
  OMX_CONFIG_EXPOSUREVALUETYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  if ((error = OMX_GetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonExposureValue, &st))){
    omxcam__error ("OMX_GetConfig - OMX_IndexConfigCommonExposureValue: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  st.xEVCompensation = (exposure_compensation << 16)/6;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonExposureValue, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonExposureValue: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_mirror (omxcam_mirror mirror, int video){
  OMX_ERRORTYPE error;
  OMX_CONFIG_MIRRORTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = video ? 71 : 72;
  st.eMirror = mirror;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonMirror, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonMirror: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_rotation (omxcam_rotation rotation, int video){
  OMX_ERRORTYPE error;
  OMX_CONFIG_ROTATIONTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = video ? 71 : 72;
  st.nRotation = rotation;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonRotate, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonRotate: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_color_effects (
    omxcam_color_effects_t* color_effects){
  OMX_ERRORTYPE error;
  OMX_CONFIG_COLORENHANCEMENTTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.bColorEnhancement = color_effects->enabled;
  st.nCustomizedU = color_effects->u;
  st.nCustomizedV = color_effects->v;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonColorEnhancement, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonColorEnhancement: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_color_denoise (omxcam_bool color_denoise){
  OMX_ERRORTYPE error;
  OMX_CONFIG_BOOLEANTYPE st;
  omxcam__omx_struct_init (st);
  st.bEnabled = !!color_denoise;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigStillColourDenoiseEnable, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigStillColourDenoiseEnable: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_metering (omxcam_metering metering){
  OMX_ERRORTYPE error;
  OMX_CONFIG_EXPOSUREVALUETYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  if ((error = OMX_GetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonExposureValue, &st))){
    omxcam__error ("OMX_GetConfig - OMX_IndexConfigCommonExposureValue: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  st.eMetering = metering;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonExposureValue, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonExposureValue: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_white_balance (omxcam_white_balance_t* white_balance){
  OMX_ERRORTYPE error;
  OMX_CONFIG_WHITEBALCONTROLTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.eWhiteBalControl = white_balance->mode;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonWhiteBalance, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonWhiteBalance: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  if (white_balance->mode == OMXCAM_WHITE_BALANCE_OFF){
    OMX_CONFIG_CUSTOMAWBGAINSTYPE gain_st;
    omxcam__omx_struct_init (gain_st);
    gain_st.xGainR = (white_balance->red_gain << 16)/1000;
    gain_st.xGainB = (white_balance->blue_gain << 16)/1000;
    if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
        OMX_IndexConfigCustomAwbGains, &gain_st))){
      omxcam__error ("OMX_SetConfig - OMX_IndexConfigCustomAwbGains: %s",
          omxcam__dump_OMX_ERRORTYPE (error));
      return -1;
    }
  }
  return 0;
}

int omxcam__camera_set_image_filter (omxcam_image_filter image_filter){
  OMX_ERRORTYPE error;
  OMX_CONFIG_IMAGEFILTERTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.eImageFilter = image_filter;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonImageFilter, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonImageFilter: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_roi (omxcam_roi_t* roi){
  OMX_ERRORTYPE error;
  OMX_CONFIG_INPUTCROPTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.xLeft = (roi->left << 16)/100;
  st.xTop = (roi->top << 16)/100;
  st.xWidth = (roi->width << 16)/100;
  st.xHeight = (roi->height << 16)/100;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigInputCropPercentages, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigInputCropPercentages: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_drc (omxcam_drc drc){
  OMX_ERRORTYPE error;
  OMX_CONFIG_DYNAMICRANGEEXPANSIONTYPE st;
  omxcam__omx_struct_init (st);
  st.eMode = drc;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigDynamicRangeExpansion, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigDynamicRangeExpansion: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_set_frame_stabilisation (omxcam_bool frame_stabilisation){
  OMX_ERRORTYPE error;
  OMX_CONFIG_FRAMESTABTYPE st;
  omxcam__omx_struct_init (st);
  st.nPortIndex = OMX_ALL;
  st.bStab = !!frame_stabilisation;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonFrameStabilisation, &st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonFrameStabilisation: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  return 0;
}

int omxcam__camera_configure_omx (
    omxcam_camera_settings_t* settings,
    int video){
  omxcam__trace ("configuring '%s' settings", omxcam__ctx.camera.name);
  
  if (omxcam__camera_set_sharpness (settings->sharpness)) return -1;
  if (omxcam__camera_set_contrast (settings->contrast)) return -1;
  if (omxcam__camera_set_brightness (settings->brightness)) return -1;
  if (omxcam__camera_set_saturation (settings->saturation)) return -1;
  if (omxcam__camera_set_exposure (settings->exposure)) return -1;
  if (omxcam__camera_set_mirror (settings->mirror, video)) return -1;
  if (omxcam__camera_set_rotation (settings->rotation, video)) return -1;
  if (omxcam__camera_set_color_effects (&settings->color_effects)){
    return -1;
  }
  if (!video && omxcam__camera_set_color_denoise (settings->color_denoise)){
    return -1;
  }
  if (omxcam__camera_set_white_balance (&settings->white_balance)) return -1;
  if (omxcam__camera_set_image_filter (settings->image_filter)) return -1;
  if (omxcam__camera_set_roi (&settings->roi)) return -1;
  if (!video && omxcam__camera_set_drc (settings->drc)) return -1;
  if (video && omxcam__camera_set_frame_stabilisation (
      settings->frame_stabilisation)){
    return -1;
  }
  
  OMX_ERRORTYPE error;
  OMX_CONFIG_EXPOSUREVALUETYPE exposure_st;
  omxcam__omx_struct_init (exposure_st);
  exposure_st.nPortIndex = OMX_ALL;
  exposure_st.eMetering = settings->metering;
  exposure_st.xEVCompensation = (settings->exposure_compensation << 16)/6;
  //Despite the name says it's in milliseconds, it's in microseconds
  exposure_st.nShutterSpeedMsec = settings->shutter_speed;
  exposure_st.bAutoShutterSpeed = !settings->shutter_speed;
  exposure_st.nSensitivity = settings->iso;
  exposure_st.bAutoSensitivity = !settings->iso;
  if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
      OMX_IndexConfigCommonExposureValue, &exposure_st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigCommonExposureValue: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__camera_is_valid_width (uint32_t width, int video){
  return width >= OMXCAM_MIN_WIDTH &&
      width <= (video ? OMXCAM_VIDEO_MAX_WIDTH : OMXCAM_STILL_MAX_WIDTH);
}

int omxcam__camera_is_valid_height (uint32_t height, int video){
  return height >= OMXCAM_MIN_HEIGHT &&
      height <= (video ? OMXCAM_VIDEO_MAX_HEIGHT : OMXCAM_STILL_MAX_HEIGHT);
}

int omxcam__camera_is_valid_sharpness (int32_t sharpness){
  return sharpness >= -100 && sharpness <= 100;
}

int omxcam__camera_is_valid_contrast (int32_t contrast){
  return contrast >= -100 && contrast <= 100;
}

int omxcam__camera_is_valid_brightness (uint32_t brightness){
  return brightness <= 100;
}

int omxcam__camera_is_valid_saturation (int32_t saturation){
  return saturation >= -100 && saturation <= 100;
}

int omxcam__camera_is_valid_exposure_compensation (
    int32_t exposure_compensation){
  return exposure_compensation >= -24 && exposure_compensation <= 24;
}

int omxcam__camera_is_valid_color_effects (uint32_t color){
  return color <= 255;
}

int omxcam__camera_is_valid_roi (uint32_t roi){
  return roi <= 100;
}

int omxcam__camera_is_valid_framerate (uint32_t framerate){
  //640x480 @90fps is the upper limit, but let the firmware deal with it
  return framerate >= 1;
}

#define OMXCAM_FN(X, DEF, fn, ret, name, name_upper_case)                      \
  ret omxcam__camera_ ## fn ## _ ## name (omxcam_ ## name name){               \
    switch (name){                                                             \
      OMXCAM_ ## name_upper_case ## _MAP (X)                                   \
      DEF (OMXCAM_ ## name_upper_case)                                         \
    }                                                                          \
  }

#define OMXCAM_CREATE_FN(_, value)                                             \
  case value: return 1;
#define OMXCAM_DEFAULT(_)                                                      \
  default: return 0;

OMXCAM_FN (OMXCAM_CREATE_FN, OMXCAM_DEFAULT, is_valid, int, iso, ISO)
OMXCAM_FN (OMXCAM_CREATE_FN, OMXCAM_DEFAULT, is_valid, int, exposure, EXPOSURE)
OMXCAM_FN (OMXCAM_CREATE_FN, OMXCAM_DEFAULT, is_valid, int, mirror, MIRROR)
OMXCAM_FN (OMXCAM_CREATE_FN, OMXCAM_DEFAULT, is_valid, int, rotation, ROTATION)
OMXCAM_FN (OMXCAM_CREATE_FN, OMXCAM_DEFAULT, is_valid, int, metering, METERING)
OMXCAM_FN (OMXCAM_CREATE_FN, OMXCAM_DEFAULT, is_valid, int, white_balance,
    WHITE_BALANCE)
OMXCAM_FN (OMXCAM_CREATE_FN, OMXCAM_DEFAULT, is_valid, int, image_filter,
    IMAGE_FILTER)
OMXCAM_FN (OMXCAM_CREATE_FN, OMXCAM_DEFAULT, is_valid, int, drc, DRC)

#undef OMXCAM_DEFAULT
#undef OMXCAM_CREATE_FN

#define OMXCAM_STR_FN(name, value)                                             \
  case value: return OMXCAM_STR(OMXCAM_ ## name);
#define OMXCAM_DEFAULT(name_upper_case)                                        \
  default: return "unknown " #name_upper_case;

OMXCAM_FN (OMXCAM_STR_FN, OMXCAM_DEFAULT, str, const char*, iso, ISO)
OMXCAM_FN (OMXCAM_STR_FN, OMXCAM_DEFAULT, str, const char*, exposure, EXPOSURE)
OMXCAM_FN (OMXCAM_STR_FN, OMXCAM_DEFAULT, str, const char*, mirror, MIRROR)
OMXCAM_FN (OMXCAM_STR_FN, OMXCAM_DEFAULT, str, const char*, rotation, ROTATION)
OMXCAM_FN (OMXCAM_STR_FN, OMXCAM_DEFAULT, str, const char*, metering, METERING)
OMXCAM_FN (OMXCAM_STR_FN, OMXCAM_DEFAULT, str, const char*, white_balance,
    WHITE_BALANCE)
OMXCAM_FN (OMXCAM_STR_FN, OMXCAM_DEFAULT, str, const char*, image_filter,
    IMAGE_FILTER)

#undef OMXCAM_DEFAULT
#undef OMXCAM_STR_FN

#undef OMXCAM_FN