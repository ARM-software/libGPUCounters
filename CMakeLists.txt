cmake_minimum_required(VERSION 3.6)

project(hwcpipe LANGUAGES C CXX)

set(PROJECT_FILES
    instrument.h
    instruments_stats.h
    measurement.h
    instruments_stats.cpp)

if(ANDROID)
    list(APPEND PROJECT_FILES        
        hwc.hpp
        hwc_names.hpp
        mali_counter.h
        mali_counter.cpp)
endif()

if(UNIX AND NOT APPLE)
    list(APPEND PROJECT_FILES
        pmu.h
        pmu_counter.h
        
        pmu.cpp
        pmu_counter.cpp)
endif()
    
source_group("\\" FILES ${PROJECT_FILES})

add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
