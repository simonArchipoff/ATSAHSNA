cmake_minimum_required(VERSION 3.5)

project(ATSAHSNA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")





Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2 # or a later release
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  QCustomPlot
  GIT_REPOSITORY https://github.com/legerch/QCustomPlot-library
  GIT_TAG 2.1.1.1
)

FetchContent_MakeAvailable(QCustomPlot)

#find_package(Catch2 2 REQUIRED)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(QT NAMES Qt6 COMPONENTS Core Widgets  LinguistTools Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Charts REQUIRED)

include(CMakePrintHelpers)

set(TS_FILES ATSAHSNA_fr_FR.ts)



#qt_standard_project_setup()

add_subdirectory(third_party/fCWT_wrapper)
add_subdirectory(backend)
add_subdirectory(signal)
add_subdirectory(view)
add_subdirectory(app)

add_executable(tests test_helpers.cpp signal/test_signal.cpp backend/test_backend.cpp )
target_link_libraries(tests PRIVATE signal Catch2::Catch2WithMain fftw3 backend)



#if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#    qt_add_executable(ATSAHSNA
#        MANUAL_FINALIZATION
#        ${PROJECT_SOURCES}
#      )
#  qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
#endif()
      
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




#target_link_libraries(ATSAHSNA PRIVATE Qt::Core fCWT backend signal view)

#install(TARGETS ATSAHSNA
#    BUNDLE DESTINATION .
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#)

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_finalize_executable(ATSAHSNA)
#endif()


