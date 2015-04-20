# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# Build library

if(${IS_RASPBERRYPI})

    add_library(MMAPGPIO_LIBRARY
    mmapGpio.cpp)

    set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} MMAPGPIO_LIBRARY PARENT_SCOPE)
    set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} MMAPGPIO_LIBRARY PARENT_SCOPE)

endif()