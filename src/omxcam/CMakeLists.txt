# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# Build library
add_library(OMXCAM_LIBRARY STATIC 
	camera.c
	core.c
	debug.c
	dump_omx.c
	error.c
	event.c
	h264.c
	internal.h
	jpeg.c
	still.c
	utils.c
	version.c
	video.c)

set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} OMXCAM_LIBRARY SCOPE)
set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} OMXCAM_LIBRARY PARENT_SCOPE)
