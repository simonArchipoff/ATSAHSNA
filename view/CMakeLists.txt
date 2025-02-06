
set(VIEW_SOURCES     qbackend.h
    qbackend.cpp

    qmeasure.h
    qmeasure.cpp

    helper_results_view.h
    helper_results_view.cpp

    BodePlot.cpp
    BodePlot.h

    mainwindow.cpp
    mainwindow.h

    widget_chatgpt.h
    widget_chatgpt.cpp

    SpectrogramPlot.h
    SpectrogramPlot.cpp

    frequencyplot.h
    frequencyplot.cpp

    harmonicsplot.h
    harmonicsplot.cpp

    responseview.h
    responseview.cpp

    qharmonicview.h
    qharmonicview.cpp

    temporalplot.h
    temporalplot.cpp

    roundrobincolor.h
    roundrobincolor.cpp

    qmeasuresview.h
    qmeasuresview.cpp

    QJackView.h
    QJackView.cpp
    vumetre.h
    vumetre.cpp

    qsoundfilewidget.h
    qsoundfilewidget.cpp
)

add_library(view ${VIEW_SOURCES})

set(VIEW_HEADERS)
foreach(file ${VIEW_SOURCES})
    if (file MATCHES "\\.(h|hpp)$")
        list(APPEND VIEW_HEADERS ${file})
    endif()
endforeach()

target_precompile_headers(view PUBLIC ${VIEW_HEADERS})
if(ENABLE_JACK)
 #   add_library(view )
endif()

target_link_libraries(view PUBLIC Qt6::Widgets backend qcustomplot)
target_include_directories(view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

