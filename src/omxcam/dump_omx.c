#include "omxcam.h"
#include "internal.h"

#define OMXCAM_DUMP_CASE(x) case x: return #x;

const char* omxcam__dump_OMX_COLOR_FORMATTYPE (OMX_COLOR_FORMATTYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatUnused)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatMonochrome)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format8bitRGB332)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format12bitRGB444)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format16bitARGB4444)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format16bitARGB1555)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format16bitRGB565)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format16bitBGR565)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format18bitRGB666)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format18bitARGB1665)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format19bitARGB1666)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format24bitRGB888)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format24bitBGR888)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format24bitARGB1887)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format25bitARGB1888)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format32bitBGRA8888)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format32bitARGB8888)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV411Planar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV411PackedPlanar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV420Planar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV420PackedPlanar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV420SemiPlanar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV422Planar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV422PackedPlanar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV422SemiPlanar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYCbYCr)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYCrYCb)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatCbYCrY)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatCrYCbY)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV444Interleaved)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatRawBayer8bit)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatRawBayer10bit)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatRawBayer8bitcompressed)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatL2)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatL4)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatL8)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatL16)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatL24)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatL32)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV420PackedSemiPlanar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUV422PackedSemiPlanar)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format18BitBGR666)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format24BitARGB6666)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format24BitABGR6666)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format32bitABGR8888)
    OMXCAM_DUMP_CASE (OMX_COLOR_Format8bitPalette)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYUVUV128)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatRawBayer12bit)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatBRCMEGL)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatBRCMOpaque)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYVU420PackedPlanar)
    OMXCAM_DUMP_CASE (OMX_COLOR_FormatYVU420PackedSemiPlanar)
    default: return "unknown OMX_COLOR_FORMATTYPE";
  }
}

const char* omxcam__dump_OMX_OTHER_FORMATTYPE (OMX_OTHER_FORMATTYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_OTHER_FormatTime);
    OMXCAM_DUMP_CASE (OMX_OTHER_FormatPower);
    OMXCAM_DUMP_CASE (OMX_OTHER_FormatStats);
    OMXCAM_DUMP_CASE (OMX_OTHER_FormatBinary);
    OMXCAM_DUMP_CASE (OMX_OTHER_FormatText);
    OMXCAM_DUMP_CASE (OMX_OTHER_FormatTextSKM2);
    OMXCAM_DUMP_CASE (OMX_OTHER_FormatText3GP5);
    default: return "unknown OMX_OTHER_FORMATTYPE";
  }
}

const char* omxcam__dump_OMX_AUDIO_CODINGTYPE (OMX_AUDIO_CODINGTYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingUnused)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingAutoDetect)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingPCM)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingADPCM)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingAMR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingGSMFR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingGSMEFR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingGSMHR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingPDCFR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingPDCEFR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingPDCHR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingTDMAFR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingTDMAEFR)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingQCELP8)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingQCELP13)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingEVRC)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingSMV)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingG711)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingG723)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingG726)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingG729)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingAAC)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingMP3)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingSBC)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingVORBIS)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingWMA)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingRA)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingMIDI)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingFLAC)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingDDP)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingDTS)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingWMAPRO)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingATRAC3)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingATRACX)
    OMXCAM_DUMP_CASE (OMX_AUDIO_CodingATRACAAL)
    default: return "unknown OMX_AUDIO_CODINGTYPE";
  }
}

const char* omxcam__dump_OMX_VIDEO_CODINGTYPE (OMX_VIDEO_CODINGTYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingUnused)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingAutoDetect)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingMPEG2)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingH263)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingMPEG4)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingWMV)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingRV)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingAVC)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingMJPEG)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingVP6)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingVP7)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingVP8)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingYUV)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingSorenson)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingTheora)
    OMXCAM_DUMP_CASE (OMX_VIDEO_CodingMVC)
    default: return "unknown OMX_VIDEO_CODINGTYPE";
  }
}

const char* omxcam__dump_OMX_IMAGE_CODINGTYPE (OMX_IMAGE_CODINGTYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingUnused)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingAutoDetect)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingJPEG)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingJPEG2K)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingEXIF)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingTIFF)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingGIF)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingPNG)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingLZW)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingBMP)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingTGA)
    OMXCAM_DUMP_CASE (OMX_IMAGE_CodingPPM)
    default: return "unknown OMX_IMAGE_CODINGTYPE";
  }
}

const char* omxcam__dump_OMX_STATETYPE (OMX_STATETYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_StateInvalid)
    OMXCAM_DUMP_CASE (OMX_StateLoaded)
    OMXCAM_DUMP_CASE (OMX_StateIdle)
    OMXCAM_DUMP_CASE (OMX_StateExecuting)
    OMXCAM_DUMP_CASE (OMX_StatePause)
    OMXCAM_DUMP_CASE (OMX_StateWaitForResources)
    default: return "unknown OMX_STATETYPE";
  }
}

const char* omxcam__dump_OMX_ERRORTYPE (OMX_ERRORTYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_ErrorNone)
    OMXCAM_DUMP_CASE (OMX_ErrorInsufficientResources)
    OMXCAM_DUMP_CASE (OMX_ErrorUndefined)
    OMXCAM_DUMP_CASE (OMX_ErrorInvalidComponentName)
    OMXCAM_DUMP_CASE (OMX_ErrorComponentNotFound)
    OMXCAM_DUMP_CASE (OMX_ErrorInvalidComponent)
    OMXCAM_DUMP_CASE (OMX_ErrorBadParameter)
    OMXCAM_DUMP_CASE (OMX_ErrorNotImplemented)
    OMXCAM_DUMP_CASE (OMX_ErrorUnderflow)
    OMXCAM_DUMP_CASE (OMX_ErrorOverflow)
    OMXCAM_DUMP_CASE (OMX_ErrorHardware)
    OMXCAM_DUMP_CASE (OMX_ErrorInvalidState)
    OMXCAM_DUMP_CASE (OMX_ErrorStreamCorrupt)
    OMXCAM_DUMP_CASE (OMX_ErrorPortsNotCompatible)
    OMXCAM_DUMP_CASE (OMX_ErrorResourcesLost)
    OMXCAM_DUMP_CASE (OMX_ErrorNoMore)
    OMXCAM_DUMP_CASE (OMX_ErrorVersionMismatch)
    OMXCAM_DUMP_CASE (OMX_ErrorNotReady)
    OMXCAM_DUMP_CASE (OMX_ErrorTimeout)
    OMXCAM_DUMP_CASE (OMX_ErrorSameState)
    OMXCAM_DUMP_CASE (OMX_ErrorResourcesPreempted)
    OMXCAM_DUMP_CASE (OMX_ErrorPortUnresponsiveDuringAllocation)
    OMXCAM_DUMP_CASE (OMX_ErrorPortUnresponsiveDuringDeallocation)
    OMXCAM_DUMP_CASE (OMX_ErrorPortUnresponsiveDuringStop)
    OMXCAM_DUMP_CASE (OMX_ErrorIncorrectStateTransition)
    OMXCAM_DUMP_CASE (OMX_ErrorIncorrectStateOperation)
    OMXCAM_DUMP_CASE (OMX_ErrorUnsupportedSetting)
    OMXCAM_DUMP_CASE (OMX_ErrorUnsupportedIndex)
    OMXCAM_DUMP_CASE (OMX_ErrorBadPortIndex)
    OMXCAM_DUMP_CASE (OMX_ErrorPortUnpopulated)
    OMXCAM_DUMP_CASE (OMX_ErrorComponentSuspended)
    OMXCAM_DUMP_CASE (OMX_ErrorDynamicResourcesUnavailable)
    OMXCAM_DUMP_CASE (OMX_ErrorMbErrorsInFrame)
    OMXCAM_DUMP_CASE (OMX_ErrorFormatNotDetected)
    OMXCAM_DUMP_CASE (OMX_ErrorContentPipeOpenFailed)
    OMXCAM_DUMP_CASE (OMX_ErrorContentPipeCreationFailed)
    OMXCAM_DUMP_CASE (OMX_ErrorSeperateTablesUsed)
    OMXCAM_DUMP_CASE (OMX_ErrorTunnelingUnsupported)
    OMXCAM_DUMP_CASE (OMX_ErrorDiskFull)
    OMXCAM_DUMP_CASE (OMX_ErrorMaxFileSize)
    OMXCAM_DUMP_CASE (OMX_ErrorDrmUnauthorised)
    OMXCAM_DUMP_CASE (OMX_ErrorDrmExpired)
    OMXCAM_DUMP_CASE (OMX_ErrorDrmGeneral)
    default: return "unknown OMX_ERRORTYPE";
  }
}

const char* omxcam__dump_OMX_EVENTTYPE (OMX_EVENTTYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_EventCmdComplete)
    OMXCAM_DUMP_CASE (OMX_EventError)
    OMXCAM_DUMP_CASE (OMX_EventMark)
    OMXCAM_DUMP_CASE (OMX_EventPortSettingsChanged)
    OMXCAM_DUMP_CASE (OMX_EventBufferFlag)
    OMXCAM_DUMP_CASE (OMX_EventResourcesAcquired)
    OMXCAM_DUMP_CASE (OMX_EventComponentResumed)
    OMXCAM_DUMP_CASE (OMX_EventDynamicResourcesAvailable)
    OMXCAM_DUMP_CASE (OMX_EventPortFormatDetected)
    OMXCAM_DUMP_CASE (OMX_EventParamOrConfigChanged)
    default: return "unkonwn OMX_EVENTTYPE";
  }
}

const char* omxcam__dump_OMX_INDEXTYPE (OMX_INDEXTYPE type){
  switch (type){
    OMXCAM_DUMP_CASE (OMX_IndexParamAudioInit)
    OMXCAM_DUMP_CASE (OMX_IndexParamVideoInit)
    OMXCAM_DUMP_CASE (OMX_IndexParamImageInit)
    OMXCAM_DUMP_CASE (OMX_IndexParamOtherInit)
    default: return "other OMX_INDEXTYPE";
  }
}

#undef OMXCAM_DUMP_CASE

void omxcam__dump_OMX_PARAM_PORTDEFINITIONTYPE (
    OMX_PARAM_PORTDEFINITIONTYPE* type){
  char domain[512];
  char domain_info[512];
  
  switch (type->eDomain){
    case OMX_PortDomainAudio:
      strcpy (domain, "OMX_PortDomainAudio");
      sprintf (domain_info,
          "    cMIMEType: %s\n"
          "    bFlagErrorConcealment: %s\n"
          "    eEncoding: %s\n",
          type->format.video.cMIMEType,
          omxcam__strbool (type->format.image.bFlagErrorConcealment),
          omxcam__dump_OMX_AUDIO_CODINGTYPE (
              type->format.audio.eEncoding));
      break;
    case OMX_PortDomainVideo:
      strcpy (domain, "OMX_PortDomainVideo");
      sprintf (domain_info,
          "    cMIMEType: %s\n"
          "    nFrameWidth: %d\n"
          "    nFrameHeight: %d\n"
          "    nStride: %d\n"
          "    nSliceHeight: %d\n"
          "    nBitrate: %d\n"
          "    xFramerate: %d\n"
          "    bFlagErrorConcealment: %s\n"
          "    eCompressionFormat: %s\n"
          "    eColorFormat: %s\n"
          , type->format.video.cMIMEType,
          type->format.video.nFrameWidth,
          type->format.video.nFrameHeight,
          type->format.video.nStride,
          type->format.video.nSliceHeight,
          type->format.video.nBitrate,
          type->format.video.xFramerate,
          omxcam__strbool (type->format.image.bFlagErrorConcealment),
          omxcam__dump_OMX_VIDEO_CODINGTYPE (
              type->format.video.eCompressionFormat),
          omxcam__dump_OMX_COLOR_FORMATTYPE (
              type->format.video.eColorFormat));
      break;
    case OMX_PortDomainImage:
      strcpy (domain, "OMX_PortDomainImage");
      sprintf (domain_info,
          "    cMIMEType: %s\n"
          "    nFrameWidth: %d\n"
          "    nFrameHeight: %d\n"
          "    nStride: %d\n"
          "    nSliceHeight: %d\n"
          "    bFlagErrorConcealment: %s\n"
          "    eCompressionFormat: %s\n"
          "    eColorFormat: %s\n"
          , type->format.image.cMIMEType,
          type->format.image.nFrameWidth,
          type->format.image.nFrameHeight,
          type->format.image.nStride,
          type->format.image.nSliceHeight,
          omxcam__strbool (type->format.image.bFlagErrorConcealment),
          omxcam__dump_OMX_IMAGE_CODINGTYPE (
              type->format.image.eCompressionFormat),
          omxcam__dump_OMX_COLOR_FORMATTYPE (
              type->format.image.eColorFormat));
      break;
    case OMX_PortDomainOther:
      strcpy (domain, "OMX_PortDomainOther");
      sprintf (domain_info,
          "    eFormat: %s\n",
          omxcam__dump_OMX_OTHER_FORMATTYPE (
              type->format.other.eFormat));
      break;
    default:
      strcpy (domain, "unknown");
      strcpy (domain_info, "    unknown");
      break;
  }
  
  printf (
      "nSize: %d\n"
      "nPortIndex: %d\n"
      "eDir: %s\n"
      "nBufferCountActual: %d\n"
      "nBufferCountMin: %d\n"
      "nBufferSize: %d\n"
      "bEnabled: %s\n"
      "bPopulated: %s\n"
      "eDomain: %s\n"
      "  format:\n"
      "%s"
      "bBuffersContiguous: %s\n"
      "nBufferAlignment: %d\n",
      type->nSize,
      type->nPortIndex,
      type->eDir == OMX_DirInput ? "input" : "output",
      type->nBufferCountActual,
      type->nBufferCountMin,
      type->nBufferSize,
      omxcam__strbool (type->bEnabled),
      omxcam__strbool (type->bPopulated),
      domain,
      domain_info,
      omxcam__strbool (type->bBuffersContiguous),
      type->nBufferAlignment);
}

void omxcam__dump_OMX_IMAGE_PARAM_PORTFORMATTYPE (
    OMX_IMAGE_PARAM_PORTFORMATTYPE* type){
  printf (
      "nSize: %d\n"
      "nPortIndex: %d\n"
      "nIndex: %d\n"
      "eCompressionFormat: %s\n"
      "eColorFormat: %s\n",
      type->nSize,
      type->nPortIndex,
      type->nIndex,
      omxcam__dump_OMX_IMAGE_CODINGTYPE (type->eCompressionFormat),
      omxcam__dump_OMX_COLOR_FORMATTYPE (type->eColorFormat));
}

void omxcam__dump_OMX_BUFFERHEADERTYPE (OMX_BUFFERHEADERTYPE* type){
  /*long long int timestamp = (long long int)type->nTimeStamp->nHighPart;
  timestamp = timestamp << 32;
  timestamp |= (long long int)type->nTimeStamp->nLowPart;

  printf (
      "nSize: %d\n"
      "nAllocLen: %d\n"
      "nFilledLen: %d\n"
      "nOffset: %d\n"
      "hMarkTargetComponent: %s\n"
      "nTickCount: %d\n"
      "nTimeStamp: %lld\n"
      "nFlags: %X\n"
      "nOutputPortIndex: %d\n"
      "nInputPortIndex: %d\n",
      type->nSize,
      type->nAllocLen,
      type->nFilledLen,
      type->nOffset,
      type->hMarkTargetComponent ? "not null" : "null (no mark)",
      type->nTickCount,
      timestamp,
      type->nFlags,
      type->nOutputPortIndex,
      type->nInputPortIndex);*/
}
