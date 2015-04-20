cmake_minimum_required( VERSION 2.8.3 )
project(Kerberos)

# ----------------------------------------------
# Check if compiling on the raspberry pi

	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
	    set(IS_RASPBERRYPI 1)   
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

# ----------------------------------------------
# Link source and tests (if enabled)

	add_subdirectory(src)
	if(TESTS_ENABLED)
		add_subdirectory(test)
	endif()