cmake_minimum_required(VERSION 3.14)

project(ATSAHSNA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")



Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2 # or a later release
)
FetchContent_MakeAvailable(Catch2)

#find_package(Catch2 2 REQUIRED)



include(CMakePrintHelpers)


set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


find_package(Qt6 COMPONENTS Core  REQUIRED)

qt_standard_project_setup()

add_subdirectory(third_party/fCWT_wrapper)
add_subdirectory(backend)
add_subdirectory(signal)

add_executable(tests test_helpers.cpp signal/test_signal.cpp )
target_link_libraries(tests PRIVATE signal Catch2::Catch2WithMain)


set(PROJECT_SOURCES
        constants.h
        helpers.h
        main.cpp
        ${TF_SOURCES}
        ${TS_FILES}
)


add_executable(ATSAHSNA ${PROJECT_SOURCES})

target_link_libraries(ATSAHSNA PRIVATE Qt6::Core tbb fCWT backend signal) #I dont know what uses tbb, fcwt probably




