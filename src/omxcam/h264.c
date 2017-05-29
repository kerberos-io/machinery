#include "omxcam.h"
#include "internal.h"

void omxcam__h264_init (omxcam_h264_settings_t* settings){
  //Decent bitrate for 1080p
  settings->bitrate = 17000000;
  //If the IDR period is set to off, only one IDR frame will be inserted at the
  //beginning of the stream
  settings->idr_period = OMXCAM_H264_IDR_PERIOD_OFF;
  settings->sei = OMXCAM_FALSE;
  settings->eede.enabled = OMXCAM_FALSE;
  settings->eede.loss_rate = 0;
  settings->qp.enabled = OMXCAM_FALSE;
  settings->qp.i = OMXCAM_H264_QP_OFF;
  settings->qp.p = OMXCAM_H264_QP_OFF;
  settings->profile = OMXCAM_H264_AVC_PROFILE_HIGH;
  settings->inline_headers = OMXCAM_FALSE;
  settings->inline_motion_vectors = OMXCAM_FALSE;
}

int omxcam__h264_validate (omxcam_h264_settings_t* settings){
  if (!omxcam__h264_is_valid_bitrate (settings->bitrate)){
    omxcam__error ("invalid 'h264.bitrate' value");
    return -1;
  }
  if (!omxcam__h264_is_valid_eede_loss_rate (settings->eede.loss_rate)){
    omxcam__error ("invalid 'h264.eede.loss_rate' value");
    return -1;
  }
  if (!omxcam__h264_is_valid_quantization (settings->qp.i)){
    omxcam__error ("invalid 'h264.eede.i' value");
    return -1;
  }
  if (!omxcam__h264_is_valid_quantization (settings->qp.p)){
    omxcam__error ("invalid 'h264.eede.p' value");
    return -1;
  }
  if (!omxcam__h264_is_valid_avc_profile (settings->profile)){
    omxcam__error ("invalid 'h264.profile' value");
    return -1;
  }
  return 0;
}

int omxcam__h264_is_valid_bitrate (uint32_t bitrate){
  return bitrate >= 1 && bitrate <= 25000000;
}

int omxcam__h264_is_valid_eede_loss_rate (uint32_t loss_rate){
  return loss_rate <= 100;
}

int omxcam__h264_is_valid_quantization (uint32_t qp){
  return qp <= 51;
}

int omxcam__h264_configure_omx (omxcam_h264_settings_t* settings){
  omxcam__trace ("configuring '%s' settings", omxcam__ctx.video_encode.name);
  
  OMX_ERRORTYPE error;
  
  if (!settings->qp.enabled){
    //Bitrate
    OMX_VIDEO_PARAM_BITRATETYPE bitrate_st;
    omxcam__omx_struct_init (bitrate_st);
    bitrate_st.eControlRate = OMX_Video_ControlRateVariable;
    bitrate_st.nTargetBitrate = settings->bitrate;
    bitrate_st.nPortIndex = 201;
    if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
        OMX_IndexParamVideoBitrate, &bitrate_st))){
      omxcam__error ("OMX_SetParameter - OMX_IndexParamVideoBitrate: %s",
          omxcam__dump_OMX_ERRORTYPE (error));
      return -1;
    }
  }else{
    //Quantization parameters
    OMX_VIDEO_PARAM_QUANTIZATIONTYPE quantization_st;
    omxcam__omx_struct_init (quantization_st);
    quantization_st.nPortIndex = 201;
    //nQpB returns an error, it cannot be modified
    quantization_st.nQpI = settings->qp.i;
    quantization_st.nQpP = settings->qp.p;
    if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
        OMX_IndexParamVideoQuantization, &quantization_st))){
      omxcam__error ("OMX_SetParameter - OMX_IndexParamVideoQuantization: %s",
          omxcam__dump_OMX_ERRORTYPE (error));
      return -1;
    }
  }
  
  //Codec
  OMX_VIDEO_PARAM_PORTFORMATTYPE format_st;
  omxcam__omx_struct_init (format_st);
  format_st.nPortIndex = 201;
  //H.264/AVC
  format_st.eCompressionFormat = OMX_VIDEO_CodingAVC;
  if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
      OMX_IndexParamVideoPortFormat, &format_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamVideoPortFormat: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //IDR period
  OMX_VIDEO_CONFIG_AVCINTRAPERIOD idr_st;
  omxcam__omx_struct_init (idr_st);
  idr_st.nPortIndex = 201;
  if ((error = OMX_GetConfig (omxcam__ctx.video_encode.handle,
      OMX_IndexConfigVideoAVCIntraPeriod, &idr_st))){
    omxcam__error ("OMX_GetConfig - OMX_IndexConfigVideoAVCIntraPeriod: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  idr_st.nIDRPeriod = settings->idr_period;
  if ((error = OMX_SetConfig (omxcam__ctx.video_encode.handle,
      OMX_IndexConfigVideoAVCIntraPeriod, &idr_st))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigVideoAVCIntraPeriod: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //SEI
  OMX_PARAM_BRCMVIDEOAVCSEIENABLETYPE sei_st;
  omxcam__omx_struct_init (sei_st);
  sei_st.nPortIndex = 201;
  sei_st.bEnable = !!settings->sei;
  if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
      OMX_IndexParamBrcmVideoAVCSEIEnable, &sei_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamBrcmVideoAVCSEIEnable: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //EEDE
  OMX_VIDEO_EEDE_ENABLE eede_st;
  omxcam__omx_struct_init (eede_st);
  eede_st.nPortIndex = 201;
  eede_st.enable = !!settings->eede.enabled;
  if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
      OMX_IndexParamBrcmEEDEEnable, &eede_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamBrcmEEDEEnable: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  OMX_VIDEO_EEDE_LOSSRATE eede_loss_rate_st;
  omxcam__omx_struct_init (eede_loss_rate_st);
  eede_loss_rate_st.nPortIndex = 201;
  eede_loss_rate_st.loss_rate = settings->eede.loss_rate;
  if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
      OMX_IndexParamBrcmEEDELossRate, &eede_loss_rate_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamBrcmEEDELossRate: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //AVC Profile
  OMX_VIDEO_PARAM_AVCTYPE avc_profile_st;
  omxcam__omx_struct_init (avc_profile_st);
  avc_profile_st.nPortIndex = 201;
  if ((error = OMX_GetParameter (omxcam__ctx.video_encode.handle,
      OMX_IndexParamVideoAvc, &avc_profile_st))){
    omxcam__error ("OMX_GetParameter - OMX_IndexParamVideoAvc: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  avc_profile_st.eProfile = settings->profile;
  if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
      OMX_IndexParamVideoAvc, &avc_profile_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamVideoAvc: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //Inline SPS/PPS
  OMX_CONFIG_PORTBOOLEANTYPE headers_st;
  omxcam__omx_struct_init (headers_st);
  headers_st.nPortIndex = 201;
  headers_st.bEnabled = !!settings->inline_headers;
  if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
      OMX_IndexParamBrcmVideoAVCInlineHeaderEnable, &headers_st))){
    omxcam__error ("OMX_SetParameter - "
        "OMX_IndexParamBrcmVideoAVCInlineHeaderEnable: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //Inline motion vectors
  OMX_CONFIG_PORTBOOLEANTYPE motion_st;
  omxcam__omx_struct_init (motion_st);
  motion_st.nPortIndex = 201;
  motion_st.bEnabled = !!settings->inline_motion_vectors;
  if ((error = OMX_SetParameter (omxcam__ctx.video_encode.handle,
      OMX_IndexParamBrcmVideoAVCInlineVectorsEnable, &motion_st))){
    omxcam__error ("OMX_SetParameter - "
        "OMX_IndexParamBrcmVideoAVCInlineVectorsEnable: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

#define OMXCAM_FN(X, name, name_upper_case)                                    \
  int omxcam__h264_is_valid_ ## name (omxcam_ ## name name){                   \
    switch (name){                                                             \
      OMXCAM_ ## name_upper_case ## _MAP (X)                                   \
      default: return 0;                                                       \
    }                                                                          \
  }

#define OMXCAM_CASE_FN(_, value)                                               \
  case value: return 1;

OMXCAM_FN (OMXCAM_CASE_FN, avc_profile, H264_AVC_PROFILE)

#undef OMXCAM_CASE_FN
#undef OMXCAM_FN