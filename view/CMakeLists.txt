
find_package(Qwt REQUIRED)

add_library(view
    BodePlot.cpp
    BodePlot.h
    TemporalPlot.cpp
    TemporalPlot.h
    qbackend.h
    qbackend.cpp
    qmeasure.h
    qmeasure.cpp
    qresults.h
    qresults.cpp
    qspectrogram.h
    qspectrogram.cpp
    qwtthings.h
    qwtthings.cpp)

target_link_libraries(view PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qwt::Qwt backend)
target_include_directories(view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
