

add_library(view
    qbackend.h
    qbackend.cpp
    mainwindow.cpp
    mainwindow.h)

target_link_libraries(view PUBLIC Qt${QT_VERSION_MAJOR}::Widgets backend)
target_include_directories(view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
