
find_package(PkgConfig)

option(ENABLE_JACK "Activer le backend Jack" OFF)
message(STATUS "Variable ENABLE_JACK: ${ENABLE_JACK}")
if(ENABLE_JACK)
    find_package(Jack)
    pkg_check_modules(JACK REQUIRED jack)
endif()



find_package(Faust REQUIRED)

set(HEADER_FAUST_QTUI ${FAUST_INCLUDE_DIR}/faust/gui/QTUI.h)
#set(HEADER_FAUST_QTUI_MOC ${CMAKE_CURRENT_BINARY_DIR}/moc_QTUI.cpp)

#include(CMakePrintHelpers)
set(BACKEND_SOURCES
    ${HEADER_FAUST_QTUI}
    backend.h
    backend.cpp

    Faust.h
    Faust.cpp


)


if(ENABLE_JACK)
    list(APPEND BACKEND_SOURCES
        Jack.h
        Jack.cpp
        RingBuffer.h
        RingBuffer.cpp

        RTModule.h

        Acquisition.h
        Sender.h
        Receiver.h
        AudioIO.h
        concurrentqueue.h
    )
list(APPEND backend_LIB_DEPENDS jack)
endif()




add_library(backend ${BACKEND_SOURCES} )
target_include_directories(backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(backend PUBLIC faust signal Qt6::Core Qt6::Widgets )



