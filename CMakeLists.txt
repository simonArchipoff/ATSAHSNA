cmake_minimum_required(VERSION 3.5)

project(ATSAHSNA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(0)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
endif()



set(CXX_STANDARD C++23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")


find_package(PkgConfig)

Include(FetchContent)



set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)



include(CMakePrintHelpers)


#qt_standard_project_setup()

#add_subdirectory(backend)
add_subdirectory(signal)

      
set(PROJECT_SOURCES
        constantes.h
        helpers.h
        commandlineparser.h
        commandlineparser.cpp
#        main.cpp
        acquisition.h
        acquisition.cpp
        ${TF_SOURCES}
        ${TS_FILES}
)






