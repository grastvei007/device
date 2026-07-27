cmake_minimum_required(VERSION 3.16)
project(device)

link_directories(${CMAKE_BINARY_DIR}/$<CONFIG>/lib)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

qt_add_library(device SHARED
    device.h
    device.cpp

    inputdevicefactory.h
    inputdevicefactory.cpp
    inputdevicemanager.h
    inputdevicemanager.cpp

    serialportsettings.h
    serialportsettings.cpp

    msg/message.h
    msg/message.cpp
    msg/messagehandler.h
    msg/messagehandler.cpp
    msg/messagehandlermanager.h
    msg/messagehandlermanager.cpp
    msg/messagepair.h
    msg/messagepair.cpp
    msg/messagereader.h
    msg/messagereader.cpp

    serialdevices/atmega.h
    serialdevices/atmega.cpp
    serialdevices/victronenergy.h
    serialdevices/victronenergy.cpp
    serialdevices/bms123electric.h
    serialdevices/bms123electric.cpp
)

target_link_libraries(device PRIVATE Qt6::Core
    Qt6::SerialPort)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Debug mode")
    target_link_libraries(device PRIVATE tagsystemd)
else()
    message("Release mode")
    target_link_libraries(device PRIVATE tagsystem)
endif()

target_include_directories(device INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(device
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin
)

add_subdirectory(tests)
