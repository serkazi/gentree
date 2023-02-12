//
// This is a sample implementation of the covering algorithm in:
// "A Uniform Approach Towards Succinct Representation of Trees",
// by Farzan and Munro
//

#ifndef GENTREE_TREE_COVERING_TREE_COVERING_H_
#define GENTREE_TREE_COVERING_TREE_COVERING_H_

#include <iostream>
#include <memory>

struct ITreeCovering {
  virtual ~ITreeCovering() = default;
  virtual void print(std::ostream& os, const size_t L) = 0;
};

std::shared_ptr<ITreeCovering> createTreeCovering(std::istream& is);

#endif //GENTREE_TREE_COVERING_TREE_COVERING_H_
