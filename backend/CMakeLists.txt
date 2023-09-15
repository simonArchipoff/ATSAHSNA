
find_package(PkgConfig)
pkg_check_modules(JACK REQUIRED jack)


find_package(Faust REQUIRED)

set(HEADER_FAUST_QTUI ${FAUST_INCLUDE_DIR}/faust/gui/QTUI.h)
#set(HEADER_FAUST_QTUI_MOC ${CMAKE_CURRENT_BINARY_DIR}/moc_QTUI.cpp)

#include(CMakePrintHelpers)
set(BACKEND_SOURCES
    ${HEADER_FAUST_QTUI}
    backend.h
    backend.cpp

    backendFaust.h
    backendFaust.cpp

    backendJack.h
    backendJack.cpp

    RingBuffer.h
    RingBuffer.cpp

    RTModule.h
    RTModule.cpp

    concurrentqueue.h
)



#bles(BACKEND_SOURCES)


add_library(backend ${BACKEND_SOURCES}
)
target_include_directories(backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(backend PUBLIC faust signal jack Qt6::Core Qt6::Widgets )

