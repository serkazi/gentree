#ifndef GENTREE__RAND_BINTREE_H_
#define GENTREE__RAND_BINTREE_H_

#include "rand_bracket_seq_iface.h"

#include "rand_utils.h"

class RandomBrackSeqImpl : public IRandomBrackSeq {
 private:
  rand_utils utils_;
  static std::string explicit_stack_phi(const std::string &w);
  std::string random_bps(size_t n);
  size_t n_;
  static bool is_balanced(const std::string &s);
 public:
  ~RandomBrackSeqImpl() override = default;
  explicit RandomBrackSeqImpl(size_t n);
  void generate(std::ostream& os) override;
};

#endif //GENTREE__RAND_BINTREE_H_