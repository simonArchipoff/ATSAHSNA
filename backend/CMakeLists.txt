
find_package(PkgConfig)



find_package(Faust REQUIRED)
pkg_search_module(SNDFILE REQUIRED sndfile IMPORTED_TARGET)


set(HEADER_FAUST_QTUI ${FAUST_INCLUDE_DIR}/faust/gui/UI.h ${FAUST_INCLUDE_DIR}/faust/gui/QTUI.h ${FAUST_INCLUDE_DIR}/faust/gui/APIUI.h)
#set(HEADER_FAUST_QTUI_MOC ${CMAKE_CURRENT_BINARY_DIR}/moc_QTUI.cpp)

#include(CMakePrintHelpers)

set(BACKEND_HEADERS
    Faust.h
    ${HEADER_FAUST_QTUI}
 #   ${HEADER_FAUST_QTUI_MOC}
    backend.h
    soundfile.h
)


set(BACKEND_SOURCES
    backend.cpp
    Faust.cpp
    soundfile.cpp
)


if(ENABLE_JACK)
list(APPEND BACKEND_HEADERS
    Jack.h
    QJack.h
    spectrummonitor.h
    RingBuffer.h
    RTModule.h
    VectorPool.h
    Acquisition.h
    Sender.h
    Receiver.h
    AudioIO.h
    concurrentqueue.h

)
list(APPEND BACKEND_SOURCES
    Jack.cpp
    QJack.cpp
    spectrummonitor.cpp
    RingBuffer.cpp

)
list(APPEND backend_LIB_DEPENDS jack)
endif()




add_library(backend ${BACKEND_SOURCES} ${BACKEND_HEADERS}

)
#target_precompile_headers(backend PUBLIC ${BACKEND_HEADERS})
target_include_directories(backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(backend PUBLIC faust signal Qt6::Core Qt6::Widgets jack sndfile)



