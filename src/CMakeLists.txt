# ----------------------------------------------
# Add subdirectory

	include_directories(${CMAKE_SOURCE_DIR}/exceptions)
	include_directories(${CMAKE_SOURCE_DIR}/include/tinyxml)
	include_directories(${CMAKE_SOURCE_DIR}/include/executor)
	include_directories(${CMAKE_SOURCE_DIR}/include/filewatcher)
	include_directories(${CMAKE_SOURCE_DIR}/include/kerberos)
	include_directories(${CMAKE_SOURCE_DIR}/include/rapidjson)
	include_directories(${CMAKE_SOURCE_DIR}/include/mmapgpio)
    include_directories(${CMAKE_SOURCE_DIR}/include/encode)
    include_directories(${CMAKE_SOURCE_DIR}/include/easylogging)

	add_subdirectory(tinyxml)
	add_subdirectory(executor)
	add_subdirectory(filewatcher)
	add_subdirectory(kerberos)
	add_subdirectory(mmapgpio)
	add_subdirectory(encode)

# ---------------------------------------
# Get configuration files

    FILE(GLOB CONFIG_FILES ${CMAKE_SOURCE_DIR}/config/*)
    FILE(GLOB SCRIPTS ${CMAKE_SOURCE_DIR}/scripts/*)
    FILE(GLOB LOG_FILE ${CMAKE_SOURCE_DIR}/logs/log.stash)

# ----------------------------------------------
# Executable + define linking libraries

	SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

    SET(BUILD_SHARED_LIBS OFF)

	SET(KERBEROS_FACTORY_ENTITIES_TEMP "")
	foreach(src ${KERBEROS_FACTORY_ENTITIES})
	    SET(KERBEROS_FACTORY_ENTITIES_TEMP ${KERBEROS_FACTORY_ENTITIES_TEMP} "kerberos/${src}")
	endforeach(src ${KERBEROS_FACTORY_ENTITIES})

    if(${PACKAGE})
        if(APPLE)
            SET(KERBEROS_FACTORY_ENTITIES_TEMP ${KERBEROS_FACTORY_ENTITIES_TEMP} ${CONFIG_FILES} ${LOG_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/kerberosio.icns)
            add_executable(kerberosio MACOSX_BUNDLE main.cpp ${KERBEROS_FACTORY_ENTITIES_TEMP})
        endif()
        IF("${CMAKE_SYSTEM}" MATCHES "Linux")
            add_executable(kerberosio main.cpp ${KERBEROS_FACTORY_ENTITIES_TEMP})
        endif()
    else()
        add_executable(kerberosio main.cpp ${KERBEROS_FACTORY_ENTITIES_TEMP})
    endif()

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

# ----------------------------------------
# Install

	# copy configuration files
    INSTALL(FILES ${CONFIG_FILES}
        DESTINATION "/etc/opt/kerberosio/config"
        PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_WRITE GROUP_READ
        WORLD_EXECUTE WORLD_WRITE WORLD_READ)

    # copy script
    INSTALL(FILES ${SCRIPTS}
        DESTINATION "/etc/opt/kerberosio/scripts"
        PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_WRITE GROUP_READ
        WORLD_EXECUTE WORLD_WRITE WORLD_READ)

    # copy log file
    INSTALL(FILES ${LOG_FILE}
        DESTINATION "/etc/opt/kerberosio/logs"
        PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_WRITE GROUP_READ
        WORLD_EXECUTE WORLD_WRITE WORLD_READ)

    # create symbol directory
    INSTALL(DIRECTORY
        DESTINATION "/etc/opt/kerberosio/symbols"
        DIRECTORY_PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_WRITE GROUP_READ
        WORLD_EXECUTE WORLD_WRITE WORLD_READ)

    # create image directory
    INSTALL(DIRECTORY
        DESTINATION "/etc/opt/kerberosio/capture/"
        DIRECTORY_PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_WRITE GROUP_READ
        WORLD_EXECUTE WORLD_WRITE WORLD_READ)

    # copy service file
    INSTALL(FILES "${CMAKE_SOURCE_DIR}/service/kerberosio.service"
        DESTINATION "/etc/systemd/system"
        PERMISSIONS
        OWNER_EXECUTE OWNER_WRITE OWNER_READ
        GROUP_EXECUTE GROUP_WRITE GROUP_READ
        WORLD_EXECUTE WORLD_WRITE WORLD_READ)

    # install binary
    INSTALL(TARGETS kerberosio
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION /usr/bin COMPONENT Runtime)

# ----------------------------------------
# CPack

    IF(APPLE)
        SET(MACOSX_BUNDLE_ICON_FILE kerberosio.icns)
        SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/kerberosio.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        SET_SOURCE_FILES_PROPERTIES(${CONFIG_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/config)
        SET_SOURCE_FILES_PROPERTIES(${LOG_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/logs)
        set(CPACK_BINARY_DRAGNDROP ON)
    ENDIF()

    IF("${CMAKE_SYSTEM}" MATCHES "Linux")
        set(CPACK_GENERATOR "DEB")
        set(CPACK_PACKAGE_NAME "kerberosio")
        set(CPACK_PACKAGE_VENDOR "kerberos.io")
        SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cédric Verstraeten")
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Kerberos is a low-budget surveillance solution, that uses computer vision algorithms to detect changes, and that can trigger other devices. Kerberos is open source so you, but also other people, can customize the source to your needs and share it with our community. It has a green footprint when deploying on the Raspberry Pi and it's easy to install, you only need to transfer the image to the SD card and you're done.")

        set(CPACK_PACKAGE_VERSION "2.2.1")
        set(CPACK_PACKAGE_VERSION_MAJOR "2")
        set(CPACK_PACKAGE_VERSION_MINOR "2")
        set(CPACK_PACKAGE_VERSION_PATCH "1")

        string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)
        find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
        if(DPKG_PROGRAM)
            execute_process(
                COMMAND ${DPKG_PROGRAM} --print-architecture
                OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
        else(DPKG_PROGRAM)
            set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${CPACK_PACKAGE_VERSION}_${CMAKE_SYSTEM_NAME}")
        endif(DPKG_PROGRAM)

        SET(CPACK_NSIS_MODIFY_PATH ON)
    ENDIF()

	INCLUDE(CPack)
