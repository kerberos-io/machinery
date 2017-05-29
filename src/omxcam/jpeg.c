#include "omxcam.h"
#include "internal.h"

void omxcam__jpeg_init (omxcam_jpeg_settings_t* settings){
  settings->quality = 75;
  settings->exif.enabled = OMXCAM_TRUE;
  settings->exif.tags = 0;
  settings->exif.valid_tags = 0;
  settings->ijg = OMXCAM_FALSE;
  settings->thumbnail.enabled = OMXCAM_TRUE;
  settings->thumbnail.width = OMXCAM_JPEG_THUMBNAIL_WIDTH_AUTO;
  settings->thumbnail.height = OMXCAM_JPEG_THUMBNAIL_HEIGHT_AUTO;
  settings->thumbnail.preview = OMXCAM_FALSE;
  settings->raw_bayer = OMXCAM_FALSE;
}

int omxcam__jpeg_validate (omxcam_jpeg_settings_t* settings){
  if (!omxcam__jpeg_is_valid_quality (settings->quality)){
    omxcam__error ("invalid 'jpeg.quality' value");
    return -1;
  }
  if (settings->thumbnail.enabled && !settings->thumbnail.preview){
    if (!omxcam__jpeg_is_valid_thumbnail (settings->thumbnail.width)){
      omxcam__error ("invalid 'jpeg.thumbnail.width' value");
      return -1;
    }
    if (!omxcam__jpeg_is_valid_thumbnail (settings->thumbnail.height)){
      omxcam__error ("invalid 'jpeg.thumbnail.height' value");
      return -1;
    }
  }
  return 0;
}

int omxcam__jpeg_is_valid_quality (uint32_t quality){
  return quality >= 1 && quality <= 100;
}

int omxcam__jpeg_is_valid_thumbnail (uint32_t dimension){
  return dimension <= 1024;
}

int omxcam__jpeg_add_tag (char* key, char* value){
  int key_length = strlen (key);
  int value_length = strlen (value);
  
  struct {
    //These two fields need to be together
    OMX_CONFIG_METADATAITEMTYPE metadata_st;
    char metadata_padding[value_length];
  } item;
  
  omxcam__omx_struct_init (item.metadata_st);
  item.metadata_st.nSize = sizeof (item);
  item.metadata_st.eScopeMode = OMX_MetadataScopePortLevel;
  item.metadata_st.nScopeSpecifier = 341;
  item.metadata_st.eKeyCharset = OMX_MetadataCharsetASCII;
  item.metadata_st.nKeySizeUsed = key_length;
  memcpy (item.metadata_st.nKey, key, key_length);
  item.metadata_st.eValueCharset = OMX_MetadataCharsetASCII;
  item.metadata_st.nValueMaxSize = sizeof (item.metadata_padding);
  item.metadata_st.nValueSizeUsed = value_length;
  memcpy (item.metadata_st.nValue, value, value_length);
  
  OMX_ERRORTYPE error;
  
  if ((error = OMX_SetConfig (omxcam__ctx.image_encode.handle,
      OMX_IndexConfigMetadataItem, &item))){
    omxcam__error ("OMX_SetConfig - OMX_IndexConfigMetadataItem: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__jpeg_configure_omx (omxcam_jpeg_settings_t* settings){
  omxcam__trace ("configuring '%s' settings", omxcam__ctx.image_encode.name);

  OMX_ERRORTYPE error;
  
  //Quality
  OMX_IMAGE_PARAM_QFACTORTYPE quality_st;
  omxcam__omx_struct_init (quality_st);
  quality_st.nPortIndex = 341;
  quality_st.nQFactor = settings->quality;
  if ((error = OMX_SetParameter (omxcam__ctx.image_encode.handle,
      OMX_IndexParamQFactor, &quality_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamQFactor: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //Disable EXIF tags
  OMX_CONFIG_BOOLEANTYPE exif_st;
  omxcam__omx_struct_init (exif_st);
  exif_st.bEnabled = !settings->exif.enabled;
  if ((error = OMX_SetParameter (omxcam__ctx.image_encode.handle,
      OMX_IndexParamBrcmDisableEXIF, &exif_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamBrcmDisableEXIF: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //Bayer data
  if (settings->raw_bayer){
    //The filename is not relevant
    char dummy[] = "dummy";
    struct {
      //These two fields need to be together
      OMX_PARAM_CONTENTURITYPE uri_st;
      char padding[5];
    } raw;
    omxcam__omx_struct_init (raw.uri_st);
    raw.uri_st.nSize = sizeof (raw);
    memcpy (raw.uri_st.contentURI, dummy, 5);
    if ((error = OMX_SetConfig (omxcam__ctx.camera.handle,
        OMX_IndexConfigCaptureRawImageURI, &raw))){
      omxcam__error ("OMX_SetConfig - OMX_IndexConfigCaptureRawImageURI: %s",
          omxcam__dump_OMX_ERRORTYPE (error));
      return -1;
    }
  }
  
  //Enable IJG table
  OMX_PARAM_IJGSCALINGTYPE ijg_st;
  omxcam__omx_struct_init (ijg_st);
  ijg_st.nPortIndex = 341;
  ijg_st.bEnabled = settings->ijg;
  if ((error = OMX_SetParameter (omxcam__ctx.image_encode.handle,
      OMX_IndexParamBrcmEnableIJGTableScaling, &ijg_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamBrcmEnableIJGTableScaling: "
        "%s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  if (!settings->exif.enabled) return 0;
  
  //EXIF tags
  //See firmware/documentation/ilcomponents/image_decode.html for valid keys
  //See http://www.media.mit.edu/pia/Research/deepview/exif.html#IFD0Tags
  //for valid keys and their description
  char timestamp[20];
  time_t now;
  struct tm ts;
  time (&now);
  ts = *localtime (&now);
  strftime (timestamp, sizeof (timestamp), "%Y:%m:%d %H:%M:%S", &ts);
  
  if (omxcam__jpeg_add_tag ("EXIF.DateTimeOriginal", timestamp)) return -1;
  if (omxcam__jpeg_add_tag ("EXIF.DateTimeDigitized", timestamp)) return -1;
  if (omxcam__jpeg_add_tag ("IFD0.DateTime", timestamp)) return -1;
  
  uint32_t i;
  for (i=0; i<settings->exif.valid_tags; i++){
    if (omxcam__jpeg_add_tag (settings->exif.tags[i].key,
        settings->exif.tags[i].value)){
      return -1;
    }
  }
  
  //Thumbnail
  OMX_PARAM_BRCMTHUMBNAILTYPE thumbnail_st;
  omxcam__omx_struct_init (thumbnail_st);
  thumbnail_st.bEnable = settings->thumbnail.enabled;
  thumbnail_st.bUsePreview = settings->thumbnail.preview;
  thumbnail_st.nWidth = settings->thumbnail.width;
  thumbnail_st.nHeight = settings->thumbnail.height;
  if ((error = OMX_SetParameter (omxcam__ctx.image_encode.handle,
      OMX_IndexParamBrcmThumbnail, &thumbnail_st))){
    omxcam__error ("OMX_SetParameter - OMX_IndexParamBrcmThumbnail: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }

  return 0;
}