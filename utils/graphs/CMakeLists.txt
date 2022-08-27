add_library(graphs Graph.cpp)
target_include_directories(graphs PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>)