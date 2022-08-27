add_library(random_brack_seq rand_bracket_seq.cpp)
target_link_libraries(random_brack_seq PUBLIC rand_utils)
target_include_directories(random_brack_seq PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)