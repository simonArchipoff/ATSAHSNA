

add_library(view
    qbackend.h
    qbackend.cpp
    qmeasure.h
    qmeasure.cpp
    BodePlot.cpp
    BodePlot.h
    mainwindow.cpp
    mainwindow.h)

target_link_libraries(view PUBLIC Qt${QT_VERSION_MAJOR}::Widgets backend Qt6::Charts)
target_include_directories(view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
