# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# Build library
add_library(FILEWATCHER_LIBRARY STATIC 
    FileWatcher.cpp
    FileWatcherLinux.cpp
    FileWatcherOSX.cpp
    FileWatcherWin32.cpp
    Guard.cpp)
set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} FILEWATCHER_LIBRARY PARENT_SCOPE)
set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} FILEWATCHER_LIBRARY PARENT_SCOPE)