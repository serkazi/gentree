project(gentree)

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(utils)
add_subdirectory(bracket_sequences)
add_subdirectory(ordinal_trees)
add_subdirectory(tree_covering)

find_package(gflags REQUIRED HINTS /usr/local/)

find_package(Protobuf REQUIRED)
message(INFO "Probuf: " ${PROTO_HDRS})
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ipc/ordinal_tree.proto)
add_library(ordinal_trees_proto ${PROTO_SRCS})
target_include_directories(ordinal_trees_proto PUBLIC $<BUILD_INTERFACE:${PROTO_HDRS}>)

protobuf_generate_python(PROTO_PY ipc/ordinal_tree.proto)
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS EXPORT_MACRO DLL_EXPORT foo.proto)
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS DESCRIPTORS PROTO_DESCS foo.proto)

add_executable(otree main.cpp)
target_link_libraries(otree PUBLIC random_ordinal_tree ordinal_trees_proto gflags)
target_link_libraries(otree PUBLIC ${Protobuf_LIBRARIES})
