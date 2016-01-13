# ----------------------------------------------
# Add subdirectory

	include_directories(${CMAKE_SOURCE_DIR}/exceptions)
	include_directories(${CMAKE_SOURCE_DIR}/include/tinyxml)
	include_directories(${CMAKE_SOURCE_DIR}/include/executor)
	include_directories(${CMAKE_SOURCE_DIR}/include/filewatcher)
	include_directories(${CMAKE_SOURCE_DIR}/include/kerberos)
	include_directories(${CMAKE_SOURCE_DIR}/include/rapidjson)
	include_directories(${CMAKE_SOURCE_DIR}/include/mmapgpio)

	add_subdirectory(tinyxml)
	add_subdirectory(executor)
	add_subdirectory(filewatcher)
	add_subdirectory(kerberos)
	add_subdirectory(mmapgpio)

# ----------------------------------------------
# Executable + define linking libraries

	SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

	SET(KERBEROS_FACTORY_ENTITIES_TEMP "")
	foreach(src ${KERBEROS_FACTORY_ENTITIES})
	    SET(KERBEROS_FACTORY_ENTITIES_TEMP ${KERBEROS_FACTORY_ENTITIES_TEMP} "kerberos/${src}")
	endforeach(src ${KERBEROS_FACTORY_ENTITIES})

	add_executable(kerberosio main.cpp ${KERBEROS_FACTORY_ENTITIES_TEMP})
	add_dependencies(kerberosio rerun ${KERBEROS_DEPENDENCIES})
	target_link_libraries(kerberosio ${KERBEROS_CORE_LIBRARY} ${KERBEROS_LIBRARIES})

# -----------------------------------------
# Write variables to parent CMakeLists.txt

	SET(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} PARENT_SCOPE)
	SET(KERBEROS_CORE_LIBRARY ${KERBEROS_CORE_LIBRARY} PARENT_SCOPE)
	SET(KERBEROS_SRC ${KERBEROS_SRC} PARENT_SCOPE)
	SET(KERBEROS_FACTORY_ENTITIES ${KERBEROS_FACTORY_ENTITIES} PARENT_SCOPE)

#----------------------------------------
# Additional options

	SET(CMAKE_BUILD_TYPE Release)
	message("Add environment variable: export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:${OPENCV_LIBRARY_DIR}")

# ----------------------------------------
# Install

	# copy configuration files
	FILE(GLOB CONFIG_FILES ${CMAKE_BINARY_DIR}/../config/*)
	INSTALL(FILES ${CONFIG_FILES}
        DESTINATION "/etc/kerberosio/config"
        PERMISSIONS
        OWNER_WRITE OWNER_READ
        GROUP_WRITE GROUP_READ
        WORLD_WRITE WORLD_READ)

    # copy log file
    INSTALL(FILES "${CMAKE_BINARY_DIR}/../logs/log.stash"
        DESTINATION "/etc/kerberosio/logs"
        PERMISSIONS
        OWNER_WRITE OWNER_READ
        GROUP_WRITE GROUP_READ
        WORLD_WRITE WORLD_READ)

    # copy service file
    INSTALL(FILES "${CMAKE_BINARY_DIR}/../service/kerberosio.service"
        DESTINATION "/etc/systemd/system"
        PERMISSIONS
        OWNER_WRITE OWNER_READ
        GROUP_WRITE GROUP_READ
        WORLD_WRITE WORLD_READ)

    # create image directory
    INSTALL(DIRECTORY
        DESTINATION /srv/capture/
        DIRECTORY_PERMISSIONS 
        OWNER_WRITE OWNER_READ
        GROUP_WRITE GROUP_READ
        WORLD_WRITE WORLD_READ)
        
    # install binary
	INSTALL(TARGETS kerberosio DESTINATION /usr/bin)

# ----------------------------------------
# CPack

	set(CPACK_GENERATOR "DEB")
	set(CPACK_PACKAGE_NAME "kerberosio")
	set(CPACK_PACKAGE_VENDOR "kerberos.io")
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cédric Verstraeten")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Kerberos is a low-budget surveillance solution, that uses computer vision algorithms to detect changes, and that can trigger other devices. Kerberos is open source so you, but also other people, can customize the source to your needs and share it with our community. It has a green footprint when deploying on the Raspberry Pi and it's easy to install, you only need to transfer the image to the SD card and you're done.")

	set(CPACK_PACKAGE_VERSION "1.1.0")
	set(CPACK_PACKAGE_VERSION_MAJOR "1")
	set(CPACK_PACKAGE_VERSION_MINOR "1")
	set(CPACK_PACKAGE_VERSION_PATCH "0")
	SET(CPACK_NSIS_MODIFY_PATH ON)

	INCLUDE(CPack)