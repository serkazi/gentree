#include "rand_utils.h"

double rand_utils::next_double() { return distribution(generator); }

std::vector<size_t> rand_utils::rand_subset(size_t N, size_t n) {
  std::vector<size_t> res(n);
  for (size_t m= 0, t= 0; m < n; ++t)
    if ((N-t)*next_double() < n-m)
      res[m++]= t;
  return res;
}