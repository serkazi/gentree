#include "ordinal_tree.pb.h"
#include "rand_ordinal_tree_iface.h"
#include "rand_ordinal_tree_from_bps.h"

#include "gflags/gflags.h"

#include <fstream>
#include <ostream>
#include <sstream>
#include <stack>

DEFINE_uint64(n, 1ull, "n the tree size to generate");
DEFINE_uint64(dx, 0ull, "start from 1 or 0?");
DEFINE_string(output, "", "output path");

void print(std::ostream &os, const random_ordinal_tree::ordinal_tree& tree) {
  os << tree.adj_size() << '\n';
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

  if ( FLAGS_output != "" ) {
    std::ofstream ofs;
    ofs.open(FLAGS_output );
    print(ofs, proto_msg);
    ofs.close();
  }
  else {
    std::ostream &os= std::cout;
    print(os, proto_msg);
    os << std::endl;
  }
}