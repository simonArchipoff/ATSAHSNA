cmake_minimum_required(VERSION 3.9)

find_package(OpenMP)

find_package(PkgConfig)

pkg_search_module(FFTWF REQUIRED fftw3f fftw3_threads IMPORTED_TARGET)

#find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(libraryheaders
    "./fCWT/src/fcwt.h"
)
set(implementationheaders 
    "./fCWT/src/main.h" 
    "./fCWT/src/rwave.h" 
    "./fCWT/src/wavelib.h" 
) 
set(librarysources 
    "./fCWT/src/fcwt.cpp"
)
set(implementationsources 
    "./fCWT/src/main.cpp" 
    "./fCWT/src/rwave.cpp"
    "./fCWT/src/wavelib.cpp"
)

set(headers ${libraryheaders} ${implementationheaders})
set(sources ${librarysources} ${implementationsources})

# targets 
set(fcwt_library_target "fCWT")


add_library(${fcwt_library_target} ${libraryheaders} ${librarysources})
target_compile_options(${fcwt_library_target} PRIVATE -mavx2 -O2 -lm)

if(OpenMP_CXX_FOUND)
    target_link_libraries(${fcwt_library_target} PUBLIC OpenMP::OpenMP_CXX)
endif()

set_target_properties(${fcwt_library_target}
    PROPERTIES
    LINKER_LANGUAGE "CXX"
    VERSION ${PROJECT_VERSION}
)


target_link_libraries(${fcwt_library_target} PRIVATE fftw3f fftw3f_threads)
target_include_directories(${fcwt_library_target} PUBLIC "./include")

