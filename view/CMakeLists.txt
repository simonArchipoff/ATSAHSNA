

add_library(view
    qbackend.h
    qbackend.cpp
    QJackView.h
    QJackView.cpp
    qmeasure.h
    qmeasure.cpp
    BodePlot.cpp
    BodePlot.h
    mainwindow.cpp
    mainwindow.h
    widget_chatgpt.h
    widget_chatgpt.cpp
    QJackView.h QJackView.cpp
    SpectrogramPlot.h
    SpectrogramPlot.cpp
)

target_link_libraries(view PUBLIC Qt${QT_VERSION_MAJOR}::Widgets backend Qt6::Charts qcustomplot)
target_include_directories(view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

