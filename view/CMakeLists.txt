

add_library(view
    qbackend.h
    qbackend.cpp

    qmeasure.h
    qmeasure.cpp
    BodePlot.cpp
    BodePlot.h
    mainwindow.cpp
    mainwindow.h
    widget_chatgpt.h
    widget_chatgpt.cpp

    SpectrogramPlot.h
    SpectrogramPlot.cpp
)

if(ENABLE_JACK)
    add_library(view QJackView.h QJackView.cpp)
endif()

target_link_libraries(view PUBLIC Qt${QT_VERSION_MAJOR}::Widgets backend qcustomplot)
target_include_directories(view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

