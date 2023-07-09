
find_package(PkgConfig)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)

#set(HEADER_FAUST_QTUI ${FAUST_INCLUDE_DIR}/faust/gui/QTUI.h)
#set(HEADER_FAUST_QTUI_MOC ${CMAKE_CURRENT_BINARY_DIR}/moc_QTUI.cpp)

#include(CMakePrintHelpers)
set(SIGNAL_SOURCES
    signalFFT.h
    signalFFT.cpp
    signalAnalysis.h
    signalAnalysis.cpp
    signalGeneration.h
    signalGeneration.cpp
    signalHarmonics.h
    signalHarmonics.cpp
    signalResponse.h
    signalResponse.cpp
    signalSpectrogram.h
    signalSpectrogram.cpp
    signalFrequencyDomain.cpp
    signalFrequencyDomain.h)



#cmake_print_variables(BACKEND_SOURCES)

add_library(signal ${SIGNAL_SOURCES})
target_include_directories(signal PUBLIC fCWT ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(signal PUBLIC fftw3 fCWT)


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
