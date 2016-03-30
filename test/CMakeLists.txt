# ----------------------------------------------
# Get all files, starting with prefix "test_"

	FILE(GLOB TEST_SRC test_*.cpp)

# ----------------------------------------------
# Include directories
	
	include_directories(${CMAKE_SOURCE_DIR}/exceptions)
	include_directories(${CMAKE_SOURCE_DIR}/include/tinyxml)
	include_directories(${CMAKE_SOURCE_DIR}/include/executor)
	include_directories(${CMAKE_SOURCE_DIR}/include/filewatcher)
	include_directories(${CMAKE_SOURCE_DIR}/include/kerberos)
	include_directories(${CMAKE_SOURCE_DIR}/include/rapidjson)
	include_directories(${CMAKE_SOURCE_DIR}/include/mmapgpio)
    include_directories(${CMAKE_SOURCE_DIR}/include/encode)
    include_directories(${CMAKE_SOURCE_DIR}/include/easylogging)
    
# -----------------------------------
# Mocks

	include_directories(${CMAKE_SOURCE_DIR}/test/mocks/kerberos)

# ----------------------------------------------
# Create test binary

	# ----------------------------------------------
	# Change path of factory entities

		SET(KERBEROS_FACTORY_ENTITIES_TEMP "")
		foreach(src ${KERBEROS_FACTORY_ENTITIES})
		    SET(KERBEROS_FACTORY_ENTITIES_TEMP ${KERBEROS_FACTORY_ENTITIES_TEMP} "../src/kerberos/${src}")
		endforeach(src ${KERBEROS_FACTORY_ENTITIES})

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/test/bin)
	add_executable(kerberos-test ${TEST_SRC} ${KERBEROS_FACTORY_ENTITIES_TEMP})
	add_dependencies(kerberos-test ${KERBEROS_TEST_DEPENDENCIES} ${KERBEROS_DEPENDENCIES})
	target_link_libraries(kerberos-test ${KERBEROS_CORE_LIBRARY} ${KERBEROS_LIBRARIES} ${GOOGLETEST_LIBRARIES} ${GOOGLEMOCK_LIBRARIES})
	add_test(testing-kerberos ${CMAKE_SOURCE_DIR}/test/bin/kerberos-test)

# ----------------------------------------------
# Create make check command

	add_custom_target(check COMMAND kerberos-test)
	add_dependencies(check kerberos-test)
	add_test(test "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target kerberos-test)

# ----------------------------------------------
# Execute script "GTEST..", create a test for each file

	include("${PROJECT_SOURCE_DIR}/test/script/gtest_add.cmake")
	gtest_add_tests("${EXECUTABLE_OUTPUT_PATH}/kerberos-test" "" ${TEST_SRC})