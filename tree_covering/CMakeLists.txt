add_library(tree_covering tree_covering.cpp)

target_include_directories(tree_covering PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_executable(treecover main.cpp)

set(GFLAGS_INCLUDE_DIR /usr/local/include)
set(GFLAGS_LIBRARY_INSTALL_DIR /usr/local/lib)

find_package(gflags REQUIRED HINTS /usr/local/)

target_link_libraries(treecover PUBLIC tree_covering gflags)