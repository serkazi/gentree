#ifndef GENTREE_ORDINAL_TREES_RAND_ORDINAL_TREE_FROM_BPS_H_
#define GENTREE_ORDINAL_TREES_RAND_ORDINAL_TREE_FROM_BPS_H_

#include "rand_ordinal_tree_iface.h"
#include "rand_bracket_seq_iface.h"
#include "Graph.h"

#include <istream>
#include <memory>
#include <ostream>
#include <string>

class RandOrdinalTreeFromBinary : public IRandomOrdinalTree {
 private:
  std::unique_ptr<Graph> g_;
  std::unique_ptr<IRandomBrackSeq> bin_tree_;
 public:
  explicit RandOrdinalTreeFromBinary(size_t n);
  void generate(std::ostream& os) override;
};

#endif //GENTREE_ORDINAL_TREES_RAND_ORDINAL_TREE_FROM_BPS_H_