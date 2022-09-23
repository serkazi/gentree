#include "ordinal_tree.pb.h"
#include "rand_ordinal_tree_iface.h"
#include "rand_ordinal_tree_from_bps.h"

#include "gflags/gflags.h"

#include <iostream>
#include <fstream>
#include <optional>
#include <ostream>
#include <random>
#include <sstream>
#include <stack>

DEFINE_uint64(n, 1ull, "n the tree size to generate");
DEFINE_uint64(dx, 0ull, "start from 1 or 0?");
DEFINE_string(output, "", "output path");
DEFINE_uint64(a, 1ull, "lower bound on weights (inclusive)");
DEFINE_uint64(b, 0ull, "upper bound on weights (inclusive)");

template<typename T>
void print(std::ostream &os,
           const random_ordinal_tree::ordinal_tree& tree,
           std::optional<T> weights= std::nullopt) {
  os << tree.adj_size() << '\n';
  if(weights) {
    auto &p = *weights;
    int wid = 0;
    for (auto x : p) {
      os << x << ' ';
      if (++wid >= 80) {
        wid = 0;
        os << '\n';
      }
    }
    os << '\n';
  }
  for(int x = 0; x < tree.adj_size(); ++x) {
    for(auto j = 0; j < tree.adj(x).to_size(); ++j) {
      const auto y= tree.adj(x).to(j);
      os << x+FLAGS_dx << ' ' << y+FLAGS_dx << '\n';
    }
  }
}

void convert(const std::string& s, random_ordinal_tree::ordinal_tree& tree) {
  const auto n = s.size() / 2;
  for(int i= 0; i < n; ++i) {
    tree.add_adj();
  }
  std::stack<unsigned int> st;
  auto V= 0ULL;
  for (auto ch : s) {
    if(ch == ')') {
      assert(not st.empty());
      const auto x = st.top();
      st.pop();
      if(not st.empty()) {
        tree.mutable_adj(st.top())->add_to(x);
      }
    } else {
      assert(ch == '(');
      st.push(V++);
    }
  }
  assert(st.empty());
  assert(V == n);
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc,&argv,true);

  auto requested = FLAGS_n;
  std::unique_ptr<IRandomOrdinalTree> rand_tree = std::make_unique<RandOrdinalTreeFromBinary>(requested);
  std::stringstream ss;
  rand_tree->generate(ss);

  random_ordinal_tree::ordinal_tree proto_msg;
  proto_msg.Clear();
  convert(ss.str(), proto_msg);

  std::vector<std::int64_t> weights;
  if(FLAGS_a <= FLAGS_b) {
    // assign weights
    weights.resize(FLAGS_n);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(FLAGS_a,FLAGS_b);
    for(auto &x : weights)
      x = dist(rng);
  }

  if ( FLAGS_output != "" ) {
    std::ofstream ofs;
    ofs.open(FLAGS_output );
    print(ofs, proto_msg, weights.empty() ? std::nullopt : std::make_optional(weights));
    ofs.close();
  }
  else {
    std::ostream &os= std::cout;
    print(os, proto_msg, weights.empty() ? std::nullopt : std::make_optional(weights));
    os << std::endl;
  }
}