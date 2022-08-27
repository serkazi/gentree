#include "rand_ordinal_tree_from_bps.h"

#include "rand_bracket_seq.h"

#include <sstream>

RandOrdinalTreeFromBinary::RandOrdinalTreeFromBinary(size_t n) {
  bin_tree_ = std::make_unique<RandomBrackSeqImpl>(n);
  std::ostringstream os;
  bin_tree_->generate(os);
  g_ = std::make_unique<Graph>(os.str());
}

void RandOrdinalTreeFromBinary::generate(std::ostream &os) {
  g_->serialize(os);
}