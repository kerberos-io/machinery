message("Bind project: Userland")

set(USERLAND_INCLUDE_DIR /opt/vc/include/)
set(USERLAND_LIBRARY_DIR /opt/vc/lib/)

include_directories(${USERLAND_INCLUDE_DIR})
link_directories(${USERLAND_LIBRARY_DIR})