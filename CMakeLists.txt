cmake_minimum_required(VERSION 3.5)

project(ATSAHSNA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wno-unused-parameter")


include(CMakePrintHelpers)


set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


find_package(Qt6 COMPONENTS Core REQUIRED)

qt_standard_project_setup()

add_subdirectory(third_party/fCWT_wrapper)
add_subdirectory(backend)

set(PROJECT_SOURCES
        constants.h
        main.cpp
        ${TF_SOURCES}
        ${TS_FILES}
)


add_executable(ATSAHSNA ${PROJECT_SOURCES})

target_link_libraries(ATSAHSNA PRIVATE Qt6::Core tbb fCWT backend) #I dont know what uses tbb, fcwt probably




