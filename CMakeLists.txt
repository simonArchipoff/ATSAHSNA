cmake_minimum_required(VERSION 3.5)

project(ATSAHSNA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(0)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
endif()


find_package(Qt6  REQUIRED COMPONENTS Core Widgets LinguistTools )

qt_standard_project_setup()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CXX_STANDARD C++23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")


find_package(PkgConfig)

Include(FetchContent)


option(ENABLE_JACK "Activer le backend Jack" ON)
message(STATUS "Variable ENABLE_JACK: ${ENABLE_JACK}")
if(ENABLE_JACK)
    #find_package(Jack)
    pkg_check_modules(JACK REQUIRED jack)
    add_definitions(-DENABLE_JACK=1)
endif()



FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2 # or a later release
)

FetchContent_MakeAvailable(Catch2)


FetchContent_Declare(
  qcustomplot
  URL https://www.qcustomplot.com/release/2.1.1/QCustomPlot-source.tar.gz
)

FetchContent_MakeAvailable(qcustomplot)
add_library(qcustomplot STATIC
    ${qcustomplot_SOURCE_DIR}/qcustomplot.cpp
)

target_include_directories(qcustomplot PUBLIC
    ${qcustomplot_SOURCE_DIR}
)

find_package(Qt6 REQUIRED COMPONENTS PrintSupport)
target_link_libraries(qcustomplot PUBLIC Qt6::Core Qt6::Widgets Qt6::PrintSupport)

#find_package(Catch2 2 REQUIRED)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)



include(CMakePrintHelpers)

set(TS_FILES ATSAHSNA_fr_FR.ts)



#qt_standard_project_setup()

add_subdirectory(third_party/fCWT_wrapper)
add_subdirectory(backend)
add_subdirectory(signal)
add_subdirectory(view)
add_subdirectory(app)


add_executable(tests test_helpers.cpp signal/test_signal.cpp backend/test_backend.cpp backend/test_rtmodule.cpp
    benchmark.h )
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






