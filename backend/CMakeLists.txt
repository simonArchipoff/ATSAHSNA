
set(HEADER_FAUST_QTUI ${FAUST_INCLUDE_DIR}/faust/gui/QTUI.h)
#set(HEADER_FAUST_QTUI_MOC ${CMAKE_CURRENT_BINARY_DIR}/moc_QTUI.cpp)

#include(CMakePrintHelpers)
set(BACKEND_SOURCES
    ${HEADER_FAUST_QTUI}
    ${CMAKE_CURRENT_SOURCE_DIR}/backend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/backendFaust.h
    ${CMAKE_CURRENT_SOURCE_DIR}/backendFaust.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backendJack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/backendJack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/concurrentqueue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/DTF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DTF.h
    ${CMAKE_CURRENT_SOURCE_DIR}/measure.h
    ${CMAKE_CURRENT_SOURCE_DIR}/measure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/acquisition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/acquisition.cpp
    PARENT_SCOPE)
#cmake_print_variables(BACKEND_SOURCES)


#add_library(transfer_function DTF.h DTF.cpp measure.h measure.cpp)
#target_include_directories(transfer_function PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#find_package(doctest REQUIRED)
#add_executable(test_signal test.cpp )
##signal.h signal.cpp DTF.h DTF.cpp ../constants.h )
##target_compile_features(test_signal PRIVATE cxx_std_20)
#target_link_libraries(test_signal transfer_function fftw3)
#
#if(0)
#set(TF_SOURCES
#    ${CMAKE_CURRENT_SOURCE_DIR}/DTF.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/measure.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/DTF.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/measure.cpp
#    PARENT_SCOPE)
#endif()
##cmake_print_variables(BACKEND_SOURCES)
