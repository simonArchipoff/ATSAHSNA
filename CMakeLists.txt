cmake_minimum_required(VERSION 3.5)

project(ATSAHSNA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")


find_package(QT NAMES Qt5 COMPONENTS Core Widgets  LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
include(CMakePrintHelpers)
find_package(QCoro5 REQUIRED COMPONENTS Core)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
#set(CMAKE_FIND_DEBUG_MODE TRUE)
find_package(Qwt REQUIRED)

find_package(Faust REQUIRED)

find_package(PkgConfig)
pkg_check_modules(JACK REQUIRED jack)

add_subdirectory(backend)

set(TS_FILES ATSAHSNA_fr_FR.ts)

set(PROJECT_SOURCES
        constants.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        view/BodePlot.cpp
        view/BodePlot.h
        view/TemporalPlot.cpp
        view/TemporalPlot.h
        view/qbackend.h
        view/qbackend.cpp
        view/qmeasure.h
        view/qmeasure.cpp
        view/qresults.h
        view/qresults.cpp
        ${BACKEND_SOURCES}
        ${TF_SOURCES}
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ATSAHSNA
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ATSAHSNA APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(ATSAHSNA SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ATSAHSNA
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(ATSAHSNA PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QCoro::Core Qwt::Qwt fftw3 faust jack)


set_target_properties(ATSAHSNA PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ATSAHSNA)
endif()
