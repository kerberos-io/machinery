# --------------------------------
# Source is compiled to a library
#   - library is linked with the main binary
#   and test binary

    set(KERBEROS_SRC
        cloud/Cloud.cpp
        capture/Capture.cpp
        capture/Stream.cpp
        capture/Image.cpp
        machinery/Machinery.cpp
        machinery/condition/Condition.cpp
        machinery/algorithm/Algorithm.cpp
        machinery/heuristic/Heuristic.cpp
        machinery/expositor/Expositor.cpp
        machinery/io/Io.cpp
        machinery/io/FileManager.cpp
        Factory.cpp
        Helper.cpp
        Kerberos.cpp
    )

# -------------------------------------------------------
# Entities that have to be registered with the factory
#   - classes are registered on compile time, so they
#   don't belong to a library. New algorithms, expositors
#   or Io classes belong here.

    set(KERBEROS_FACTORY_ENTITIES
        cloud/S3.cpp
        cloud/GoogleDrive.cpp
        capture/USBCamera.cpp
        capture/IPCamera.cpp
        capture/VideoCapture.cpp
        machinery/condition/Time.cpp
        machinery/condition/Enabled.cpp
        machinery/algorithm/DifferentialCollins.cpp
        machinery/algorithm/BackgroundSubtraction.cpp
        machinery/expositor/RectangleExpositor.cpp
        machinery/expositor/HullExpositor.cpp
        machinery/heuristic/Sequence.cpp
        machinery/heuristic/Counter.cpp
        machinery/io/IoDisk.cpp
        machinery/io/IoVideo.cpp
        machinery/io/IoTCP.cpp
        machinery/io/IoWebhook.cpp
        machinery/io/IoScript.cpp
    )

    set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} opencv restclient)
    set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} ${OPENCV_LIBRARIES} ${RESTCLIENT_LIBRARIES} pthread)


# -------------------------------------------
# If source is compiled on a Raspberry PI
#   - We can use the PI cameraboard
#   - We can use the GPIO pins

    if(${IS_RASPBERRYPI})

        set(KERBEROS_FACTORY_ENTITIES
            ${KERBEROS_FACTORY_ENTITIES}
            capture/RaspiCamera.cpp
            machinery/io/IoGPIO.cpp
        )
        set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} raspicamera)
        set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} ${RASPBERRYPI_LIBRARIES})
        
    endif()

# -------------------------------------------
# Write variables to parent CMakeLists.txt

    set(KERBEROS_SRC ${KERBEROS_SRC} PARENT_SCOPE)
    set(KERBEROS_FACTORY_ENTITIES ${KERBEROS_FACTORY_ENTITIES} PARENT_SCOPE)
    set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES}  PARENT_SCOPE)
    set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} PARENT_SCOPE)

# -------------------------------------------
# Create a library of the KERBEROS_SRC files

    add_library(KERBEROS_CORE STATIC ${KERBEROS_SRC})
    target_link_libraries(KERBEROS_CORE ${KERBEROS_LIBRARIES})
    add_dependencies(KERBEROS_CORE ${KERBEROS_DEPENDENCIES})
    set(KERBEROS_CORE_LIBRARY ${KERBEROS_CORE_LIBRARY} KERBEROS_CORE PARENT_SCOPE)
