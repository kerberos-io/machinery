cmake_minimum_required( VERSION 2.8.3 )
project(Kerberos)

# ----------------------------------------------
# Check if compiling on the raspberry pi

	set(IS_RASPBERRYPI 0)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		FIND_LIBRARY(mmalcore_LIBS NAMES mmal_core PATHS /opt/vc/lib)
		FIND_LIBRARY(mmalutil_LIBS NAMES mmal_util PATHS /opt/vc/lib)
		FIND_LIBRARY(mmal_LIBS NAMES mmal PATHS /opt/vc/lib)

		if( ( mmal_LIBS ) AND ( mmalutil_LIBS ) AND ( mmalcore_LIBS ) )
	    	set(IS_RASPBERRYPI 1)
	  endif()
	endif()
	add_definitions(-DIS_RASPBERRYPI=${IS_RASPBERRYPI})

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

	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wuninitialized -std=gnu++11 -Wno-pmf-conversions -Wno-unused-result" )
	set( CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -Wall -std=gnu11" )
	set( CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -Wall -std=gnu11" )

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
