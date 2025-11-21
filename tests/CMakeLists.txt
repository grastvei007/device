cmake_minimum_required(VERSION 3.16)
project(test_device)

include(GoogleTest)

set(CMAKE_CXX_STANDARD 14)

set_target_properties(PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
link_directories(${CMAKE_BINARY_DIR}/$<CONFIG>/lib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gtest REQUIRED gtest>=1.8.1)

SET(CMAKE_CXX_FLAGS -pthread)
enable_testing()

include_directories(${gtest_INCLUDE_DIRS})

add_executable(test_device
    main.cpp
    test_message.cpp
    )


target_link_libraries(test_device ${gtest_LIBRARIES} device tagsystem)

target_include_directories(test_device INTERFACE ../)
target_include_directories(test_device INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


set_target_properties(test_device
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin
)

