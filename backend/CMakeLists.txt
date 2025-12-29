
find_package(PkgConfig)


#include(CMakePrintHelpers)

set(BACKEND_HEADERS
)

set(BACKEND_SOURCES
)


if(ENABLE_JACK)
list(APPEND BACKEND_HEADERS
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
    spectrummonitor.cpp
    RingBuffer.cpp
)
list(APPEND backend_LIB_DEPENDS jack)
endif()




add_library(backend ${BACKEND_SOURCES} ${BACKEND_HEADERS})
#target_precompile_headers(backend PUBLIC ${BACKEND_HEADERS})
target_include_directories(backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(backend PUBLIC  signal sndfile)



