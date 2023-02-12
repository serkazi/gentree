//
//
#include "tree_covering.h"

#include "gflags/gflags.h"

#include <iostream>

DEFINE_uint64(L, 1ull, "L tree covering parameter -- mini-tree component size");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc,&argv,true);

  std::istream& is = std::cin;
  auto ptr = createTreeCovering(is);

  std::ostream& os = std::cout;
  ptr->print(os, FLAGS_L);
  return 0;
}
