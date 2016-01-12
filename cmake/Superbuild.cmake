include( ExternalProject )

# --------------------------------------------------------------------------
# Look for GIT, check if it's installed

    find_package(Git)

    if(NOT GIT_FOUND)
      message(ERROR "Cannot find git. git is required for Kerberos")
    endif()

    option(USE_GIT_PROTOCOL ON)
    if(NOT USE_GIT_PROTOCOL)
      set(git_protocol "http")
    else()
      set(git_protocol "git")
    endif()

# --------------------------------------------------------------------------
# Testing with Googletest

    if(${TESTS_ENABLED})
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-Googletest.cmake)
        set(KERBEROS_TEST_DEPENDENCIES ${KERBEROS_TEST_DEPENDENCIES} googletest)
    endif()

# --------------------------------------------------------------------------
# OpenCV

    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-OpenCV.cmake)
    set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} opencv)

    if(${IS_RASPBERRYPI})

        # --------------------------------------------------------------------------
        # RaspiCam
        #

        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-RaspiCam.cmake)
        set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} raspicamera)

    endif()
    
# --------------------------------------------------------------------------
# Restclient

    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-Restclient.cmake)
    set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} restclient)
