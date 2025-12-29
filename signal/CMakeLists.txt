
find_package(PkgConfig)


find_package(kissfft CONFIG REQUIRED)



#set(HEADER_FAUST_QTUI ${FAUST_INCLUDE_DIR}/faust/gui/QTUI.h)
#set(HEADER_FAUST_QTUI_MOC ${CMAKE_CURRENT_BINARY_DIR}/moc_QTUI.cpp)

#include(CMakePrintHelpers)
set(SIGNAL_HEADERS
    FFT.h
    Analysis.h
    Generation.h
    Harmonics.h
    Response.h
    FrequencyDomain.h
)
set(SIGNAL_SOURCES
    FFT.cpp
    Analysis.cpp
    Generation.cpp
    Harmonics.cpp
    Response.cpp
    FrequencyDomain.cpp
    ${SIGNAL_HEADERS}
)



#cmake_print_variables(BACKEND_SOURCES)

add_library(signal ${SIGNAL_SOURCES})
target_precompile_headers(signal PUBLIC ${SIGNAL_HEADERS})
target_link_libraries(signal PRIVATE kissfft::kissfft-float)
