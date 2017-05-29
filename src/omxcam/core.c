#include "omxcam.h"
#include "internal.h"

OMX_ERRORTYPE event_handler (
    OMX_HANDLETYPE comp,
    OMX_PTR app_data,
    OMX_EVENTTYPE event,
    OMX_U32 data1,
    OMX_U32 data2,
    OMX_PTR event_data){
  omxcam__component_t* component = (omxcam__component_t*)app_data;
  omxcam__event evt = -1;
  OMX_ERRORTYPE error = OMX_ErrorNone;
  
  switch (event){
    case OMX_EventCmdComplete:
      switch (data1){
        case OMX_CommandStateSet:
          omxcam__trace ("event: OMX_CommandStateSet, state: %s",
              omxcam__dump_OMX_STATETYPE (data2));
          evt = OMXCAM_EVENT_STATE_SET;
          break;
        case OMX_CommandPortDisable:
          omxcam__trace ("event: OMX_CommandPortDisable, port: %d", data2);
          evt = OMXCAM_EVENT_PORT_DISABLE;
          break;
        case OMX_CommandPortEnable:
          omxcam__trace ("event: OMX_CommandPortEnable, port: %d", data2);
          evt = OMXCAM_EVENT_PORT_ENABLE;
          break;
        case OMX_CommandFlush:
          omxcam__trace ("event: OMX_CommandFlush, port: %d", data2);
          evt = OMXCAM_EVENT_FLUSH;
          break;
        case OMX_CommandMarkBuffer:
          omxcam__trace ("event: OMX_CommandMarkBuffer, port: %d", data2);
          evt = OMXCAM_EVENT_MARK_BUFFER;
          break;
      }
      break;
    case OMX_EventError:
      omxcam__trace ("event: %s", omxcam__dump_OMX_ERRORTYPE (data1));
      omxcam__error ("OMX_EventError: %s", omxcam__dump_OMX_ERRORTYPE (data1));
      evt = OMXCAM_EVENT_ERROR;
      error = data1;
      break;
    case OMX_EventMark:
      omxcam__trace ("event: OMX_EventMark");
      evt = OMXCAM_EVENT_MARK;
      break;
    case OMX_EventPortSettingsChanged:
      omxcam__trace ("event: OMX_EventPortSettingsChanged, port: %d", data1);
      evt = OMXCAM_EVENT_PORT_SETTINGS_CHANGED;
      break;
    case OMX_EventParamOrConfigChanged:
      omxcam__trace ("event: OMX_EventParamOrConfigChanged, data1: %d, data2: "
          "%X", data1, data2);
      evt = OMXCAM_EVENT_PARAM_OR_CONFIG_CHANGED;
      break;
    case OMX_EventBufferFlag:
      omxcam__trace ("event: OMX_EventBufferFlag, port: %d", data1);
      evt = OMXCAM_EVENT_BUFFER_FLAG;
      break;
    case OMX_EventResourcesAcquired:
      omxcam__trace ("event: OMX_EventResourcesAcquired");
      evt = OMXCAM_EVENT_RESOURCES_ACQUIRED;
      break;
    case OMX_EventDynamicResourcesAvailable:
      omxcam__trace ("event: OMX_EventDynamicResourcesAvailable");
      evt = OMXCAM_EVENT_DYNAMIC_RESOURCES_AVAILABLE;
      break;
    default:
      //This should never execute, log and ignore
      omxcam__error ("event: unknown (%X)", event);
      return OMX_ErrorNone;
  }
  
  if (omxcam__event_wake (component, evt, error)){
    omxcam__event_error (component);
  }
  
  return OMX_ErrorNone;
}

OMX_ERRORTYPE fill_buffer_done (
    OMX_HANDLETYPE comp,
    OMX_PTR app_data,
    OMX_BUFFERHEADERTYPE* buffer){
  omxcam__component_t* component = (omxcam__component_t*)app_data;
  
  omxcam__trace ("event: FillBufferDone");
  if (omxcam__event_wake (component, OMXCAM_EVENT_FILL_BUFFER_DONE,
      OMX_ErrorNone)){
    omxcam__event_error (component);
  }
  
  return OMX_ErrorNone;
}

int omxcam__component_port_enable (
    omxcam__component_t* component,
    uint32_t port){
  omxcam__trace ("enabling port %d ('%s')", port, component->name);
  
  OMX_ERRORTYPE error;
  
  if ((error = OMX_SendCommand (component->handle, OMX_CommandPortEnable,
      port, 0))){
    omxcam__error ("OMX_SendCommand: %s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__component_port_disable (
    omxcam__component_t* component,
    uint32_t port){
  omxcam__trace ("disabling port %d ('%s')", port, component->name);
  
  OMX_ERRORTYPE error;
  
  if ((error = OMX_SendCommand (component->handle, OMX_CommandPortDisable,
      port, 0))){
    omxcam__error ("OMX_SendCommand: %s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__component_init (omxcam__component_t* component){
  omxcam__trace ("initializing component '%s'", component->name);

  OMX_ERRORTYPE error;
  
  if (omxcam__event_create (component)) return -1;
  
  OMX_CALLBACKTYPE callbacks;
  callbacks.EventHandler = event_handler;
  callbacks.FillBufferDone = fill_buffer_done;
  
  if ((error = OMX_GetHandle (&component->handle, component->name, component,
      &callbacks))){
    omxcam__error ("OMX_GetHandle: %s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  //Disable all the ports
  OMX_INDEXTYPE component_types[] = {
    OMX_IndexParamAudioInit,
    OMX_IndexParamVideoInit,
    OMX_IndexParamImageInit,
    OMX_IndexParamOtherInit
  };
  OMX_PORT_PARAM_TYPE ports_st;
  omxcam__omx_struct_init (ports_st);

  int i;
  for (i=0; i<4; i++){
    if ((error = OMX_GetParameter (component->handle, component_types[i],
        &ports_st))){
      omxcam__error ("OMX_GetParameter - %s: %s",
          omxcam__dump_OMX_ERRORTYPE (error),
          omxcam__dump_OMX_INDEXTYPE (component_types[i]));
      return -1;
    }
    
    OMX_U32 port;
    for (port=ports_st.nStartPortNumber;
        port<ports_st.nStartPortNumber + ports_st.nPorts; port++){
      if (omxcam__component_port_disable (component, port)) return -1;
      if (omxcam__event_wait (component, OMXCAM_EVENT_PORT_DISABLE, 0, 0)){
        return -1;
      }
    }
  }
  
  return 0;
}

int omxcam__component_deinit (omxcam__component_t* component){
  omxcam__trace ("deinitializing component '%s'", component->name);
  
  OMX_ERRORTYPE error;
  
  if (omxcam__event_destroy (component)) return -1;

  if ((error = OMX_FreeHandle (component->handle))){
    omxcam__error ("OMX_FreeHandle: %s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__component_change_state (
    omxcam__component_t* component,
    omxcam__state state){
  omxcam__trace ("changing '%s' state to %s", component->name,
      omxcam__dump_OMX_STATETYPE (state));
  
  OMX_ERRORTYPE error;
  
  if ((error = OMX_SendCommand (component->handle, OMX_CommandStateSet, state,
      0))){
    omxcam__error ("OMX_SendCommand: %s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__buffer_alloc (omxcam__component_t* component, uint32_t port){
  omxcam__trace ("allocating '%s' output buffer", component->name);
  
  OMX_ERRORTYPE error;
  
  OMX_PARAM_PORTDEFINITIONTYPE def_st;
  omxcam__omx_struct_init (def_st);
  def_st.nPortIndex = port;
  if ((error = OMX_GetParameter (component->handle,
      OMX_IndexParamPortDefinition, &def_st))){
    omxcam__error ("OMX_GetParameter - OMX_IndexParamPortDefinition: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  if ((error = OMX_AllocateBuffer (component->handle,
      &omxcam__ctx.output_buffer, port, 0, def_st.nBufferSize))){
    omxcam__error ("OMX_AllocateBuffer: %s",
        omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__buffer_free (omxcam__component_t* component, uint32_t port){
  omxcam__trace ("releasing '%s' output buffer", component->name);
  
  OMX_ERRORTYPE error;
  
  if ((error = OMX_FreeBuffer (component->handle, port,
      omxcam__ctx.output_buffer))){
    omxcam__error ("OMX_FreeBuffer: %s", omxcam__dump_OMX_ERRORTYPE (error));
    return -1;
  }
  
  return 0;
}

int omxcam__exit (int code){
  omxcam__ctx.state.running = 0;
  omxcam__ctx.state.joined = 0;
  omxcam__ctx.state.stopping = 0;
  omxcam__ctx.state.ready = 0;
  omxcam__ctx.video = 0;
  return code;
}

int omxcam__exit_npt (int code){
  omxcam__ctx.no_pthread = 0;
  omxcam__ctx.state.running = 0;
  omxcam__ctx.state.stopping = 0;
  omxcam__ctx.state.ready = 0;
  omxcam__ctx.video = 0;
  return code;
}

int omxcam__init (){
  omxcam__ctx.camera.name = OMXCAM_CAMERA_NAME;
  omxcam__ctx.image_encode.name = OMXCAM_IMAGE_ENCODE_NAME;
  omxcam__ctx.video_encode.name = OMXCAM_VIDEO_ENCODE_NAME;
  omxcam__ctx.null_sink.name = OMXCAM_NULL_SINK_NAME;
  
  bcm_host_init ();
  
  if (omxcam__camera_check ()){
    omxcam__set_last_error (OMXCAM_ERROR_CAMERA_MODULE);
    return -1;
  }
  
  OMX_ERRORTYPE error;
  
  if ((error = OMX_Init ())){
    omxcam__error ("OMX_Init: %s", omxcam__dump_OMX_ERRORTYPE (error));
    omxcam__set_last_error (OMXCAM_ERROR_INIT);
    return -1;
  }
  
  return 0;
}

int omxcam__deinit (){
  OMX_ERRORTYPE error;
  
  if ((error = OMX_Deinit ())){
    omxcam__error ("OMX_Deinit: %s", omxcam__dump_OMX_ERRORTYPE (error));
    omxcam__set_last_error (OMXCAM_ERROR_DEINIT);
    return -1;
  }

  bcm_host_deinit ();
  
  return 0;
}