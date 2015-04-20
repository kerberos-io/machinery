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

	add_executable(kerberos main.cpp ${KERBEROS_FACTORY_ENTITIES_TEMP})
	add_dependencies(kerberos ${KERBEROS_DEPENDENCIES})
	target_link_libraries(kerberos ${KERBEROS_CORE_LIBRARY} ${KERBEROS_LIBRARIES})

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