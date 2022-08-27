#ifndef GENTREE__RAND_BINTREE_IFACE_H_
#define GENTREE__RAND_BINTREE_IFACE_H_

#include <ostream>

class IRandomBrackSeq {
 public:
  virtual ~IRandomBrackSeq() = default;
  virtual void generate(std::ostream& os) = 0;
};

#endif //GENTREE__RAND_BINTREE_IFACE_H_