cmake_minimum_required( VERSION 2.8.3 )
project(Kerberos)

# ----------------------------------------------
# Check if compiling on the raspberry pi
	
	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		FIND_LIBRARY(mmalcore_LIBS NAMES mmal_core PATHS /opt/vc/lib)
		FIND_LIBRARY(mmalutil_LIBS NAMES mmal_util PATHS /opt/vc/lib)
		FIND_LIBRARY(mmal_LIBS NAMES mmal PATHS /opt/vc/lib)

		if( ( mmal_LIBS ) AND ( mmalutil_LIBS ) AND ( mmalcore_LIBS ) )
	    	set(IS_RASPBERRYPI 1)
	    endif()
	endif()

# ----------------------------------------------
# Enable MongoDB

	set(ENABLE_MONGODB 0)

# ----------------------------------------------
# Enable tests

	set(TESTS_ENABLED 1)
	if(TESTS_ENABLED)
		include(CTest)
	endif()
	
# ----------------------------------------------
# Variables

	set(KERBEROS_DEPENDENCIES "")
	set(KERBEROS_TEST_DEPENDENCIES "")
	set(KERBEROS_LIBRARIES "")
	set(KERBEROS_CORE_LIBRARY "")
	set(KERBEROS_TEST_LIBRARY "")
	set(KERBEROS_SRC "")
	set(KERBEROS_FACTORY_ENTITIES "")

# ----------------------------------------------
# Build all the dependencies

	include(${CMAKE_SOURCE_DIR}/cmake/Superbuild.cmake)

# ---------------------------
# XCode system library

	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		link_directories("/usr/local/lib")
	endif()

# ---------
# Package

	set(PACKAGE 0 CACHE STRING "Should the project be packaged?")
    
# ----------------------------------------------
# Link source and tests (if enabled)

	add_subdirectory(src)
	if(TESTS_ENABLED)
		add_subdirectory(test)
	endif()