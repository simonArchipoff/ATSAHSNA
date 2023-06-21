
add_library(app
    delegate.cpp
    delegate.h)

target_link_libraries(app PUBLIC Qt${QT_VERSION_MAJOR}::Widgets backend view)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable(atsahsna-gui main.cpp)
target_link_libraries(atsahsna-gui PRIVATE  app signal backend view)
