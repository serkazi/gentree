#ifndef GENTREE__RAND_UTILS_H_
#define GENTREE__RAND_UTILS_H_

#include <random>
#include <vector>

class rand_utils {
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution= std::uniform_real_distribution<double>(0.00,1.00);
  double next_double();
 public:
  rand_utils();
  std::vector<size_t> rand_subset(size_t N, size_t n);
};

#endif //GENTREE__RAND_UTILS_H_