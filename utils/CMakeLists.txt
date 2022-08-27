add_library(rand_utils rand_utils.cpp)
target_include_directories(rand_utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>)

add_subdirectory(graphs)