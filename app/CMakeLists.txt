
add_library(app
    delegate.cpp
    delegate.h
    MeasureStore.h
    MeasureStore.cpp)

find_package(Qt6 REQUIRED COMPONENTS Concurrent)
target_link_libraries(app PUBLIC  Qt6::Core Qt6::Widgets Qt6::Concurrent  backend view)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable(atsahsna-gui main.cpp)
target_link_libraries(atsahsna-gui PRIVATE  app signal backend view)
