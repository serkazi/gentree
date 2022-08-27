#ifndef GENTREE_ORDINAL_TREES_RAND_ORDINAL_TREE_IFACE_H_
#define GENTREE_ORDINAL_TREES_RAND_ORDINAL_TREE_IFACE_H_

#include <ostream>

class IRandomOrdinalTree {
 public:
  virtual ~IRandomOrdinalTree() = default;
  virtual void generate(std::ostream& os) = 0;
};

#endif //GENTREE_ORDINAL_TREES_RAND_ORDINAL_TREE_IFACE_H_
