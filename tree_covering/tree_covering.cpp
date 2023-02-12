//
//

#include "tree_covering.h"

#include <array>
#include <cassert>
#include <map>
#include <optional>
#include <set>
#include <utility>
#include <vector>

using node_type = int;
using arc_type = int;
using size_type = int;
using arc_t = std::pair<int,int>;

#define N (100'000)

namespace {

  enum class Type {
    kTemporary,
    kPermanent
  };

  struct Component {
    bool contains_root = false;
    std::vector<size_type> edge_idx{};
    Type type = Type::kTemporary;
    std::optional<node_type> root = std::nullopt;
  };

  class ArcManager {
    std::map<arc_t, size_type> m_arc_id;
    std::map<size_type, arc_t> m_rev_map;
   public:
    size_type add(arc_t arc) {
      if (m_arc_id.count(arc)) {
        return m_arc_id.at(arc);
      }
      auto k = m_arc_id.size();
      m_rev_map[m_arc_id[arc] = k++] = arc;
      return k-1;
    }
    void clear() {
      m_arc_id.clear();
    }
    size_type lookup(const arc_t& arc) const {
      if (not m_arc_id.count(arc)) {
        return -1;
      }
      return m_arc_id.at(arc);
    }
    node_type destinationOf(size_type idx) const {
      return m_rev_map.at(idx).second;
    }
    node_type sourceOf(size_type idx) const {
      return m_rev_map.at(idx).first;
    }
  };

  class TreeCovering : public ITreeCovering {
    size_t n;
    ArcManager m_manager;
    std::vector<size_type> m_adj[N];
    std::array<size_type, N> m_card;
    std::array<size_type, N> m_parent;
    std::array<bool, N> m_seen;
    std::vector<Component> m_permanent_components;

    size_type calcCard(node_type x) {
      if (m_seen[x]) {
        return m_card[x];
      }
      m_seen[x] = true, m_card[x] = 1; // including itself
      for (const auto pr : m_adj[x]) {
        const auto y = m_manager.destinationOf(pr);
        if (m_seen[y]) {
          continue ;
        }
        m_parent[y] = pr, m_card[x] += calcCard(y);
      }
      return m_card[x];
    }

    // TODO: make this use std::optional
    std::vector<node_type> childrenOf(node_type x) {
      std::vector<node_type> children;
      for (const auto pr : m_adj[x]) {
        const auto y = m_manager.destinationOf(pr);
        assert(x != y);
        if (m_parent[y] == pr) {
          children.push_back(y);
        }
      }
      return children;
    }

    static Component singleton(node_type src, Type type) {
      Component cmp;
      cmp.type = type;
      cmp.contains_root = true;
      cmp.root = src;
      return cmp;
    }

    std::vector<Component> caseOne(const node_type src, const std::vector<node_type>& children,
                                   const std::map<node_type, Component>& temporary,
                                   const size_t L) {

      if (children.empty()) {
        return std::vector<Component>{singleton(src, Type::kTemporary)};
      }

      std::vector<Component> components;
      for (int i = 0, j; i < children.size(); i = j) {
        size_type acc = 0;
        for (j = i; j < children.size() and acc <= 2*L; ++j) {
          assert(temporary.count(children[j]));
          const auto &cmp = temporary.at(children[j]);
          acc += 1, acc += cmp.edge_idx.size();
        }

        Component c;
        c.contains_root = true, c.type = Type::kPermanent;
        c.edge_idx.clear(), c.root = src;
        for (int k = i; k < j; ++k) {
          // Add the edge that leads to a child:
          c.edge_idx.push_back(m_parent[children[k]]);
          const auto &cmp = temporary.at(children[k]);
          // TODO: re-write as an overloaded "+=" operator
          for (auto l = 0; l < cmp.edge_idx.size(); ++l) {
            c.edge_idx.push_back(cmp.edge_idx[l]);
          }
        }
        components.push_back(std::move(c));
      }
      if (components.size() == 1) {
          components.back().type = Type::kTemporary;
          return components;
      }
      assert(components.size() >= 2);
      m_permanent_components.insert(m_permanent_components.end(), components.begin(), components.end());
      return {};
    }

    // Each recursive call decomposes a tree rooted at a node
    // and returns component subtrees
    std::vector<Component> decompose(node_type src, const size_type L) {
      auto children = childrenOf(src);

      // First, we recursively decompose the trees rooted at the children
      std::map<node_type, Component> temporary;
      for (const auto y : children) {
        auto components = decompose(y,L);
        for (auto &c : components) {
          if (c.contains_root) {
            c.type = Type::kTemporary;
            temporary[y] = c;
          } else {
            c.type = Type::kPermanent;
            m_permanent_components.push_back(c);
          }
        }
      }

      // Find the heavy children
      std::vector<size_type> heavy_child_idx;
      for (auto i = 0; i < children.size(); ++i) {
        if (m_card[children[i]] >= L) {
          heavy_child_idx.push_back(i);
        }
      }

      // Case 1: "src" has no heavy children
      if (heavy_child_idx.empty()) {
        return caseOne(src, children, temporary, L);
      }

      // Case 2: "src" has only one heavy child
      if (heavy_child_idx.size() == 1) {
        // We proceed analogously to Case 1, except for the situation
        // when the component containing the only heavy child u[i] has been
        // declared permanent. In this case, we simply ignore it and proceed
        // directly from the u[i-1] to u[i+1]
        const auto heavy_child = children[heavy_child_idx.back()];
        if (not temporary.count(heavy_child)) {
          // We simply remove the corresponding child:
          children.erase(std::remove(children.begin(), children.end(), heavy_child),
                        children.end());
        }
        return caseOne(src, children, temporary, L);
      }

      // Case 3: Node "src" is a branching node.
      //
      // The situation here is that the root has several heavy children.
      // First, we make all the components containing these heavy nodes
      // as permanent. If after this no child is left, the root itself
      // becomes a permanent singleton component. Otherwise, we process
      // each of the remaining ranges as in Case 1.

      std::set<node_type> heavies;
      for (auto hv : heavy_child_idx) {
        heavies.insert(children[hv]);
      }
      assert(heavies.size() >= 2);
      // First, we declare permanent all the components containing the heavy nodes:
      // Note: Since "m_permanent" is being populated directly from the method,
      // we need to walk only through the "temporary" map:
      std::set<node_type> excluded;
      for (auto &[a,b] : temporary) {
        if (heavies.count(a)) {
          assert(b.type == Type::kTemporary);
          b.type = Type::kPermanent;
          excluded.insert(a);
          m_permanent_components.push_back(b);
        }
      }
      for (auto x : excluded) {
        auto ok = temporary.erase(x);
        assert(ok);
      }

      // If in fact all the children left were heavy, "src" is by itself
      // is a permanent single-node component:
      if (children.size() == heavies.size()) {
        m_permanent_components.push_back(singleton(src, Type::kPermanent));
        return {};
      }

      // Otherwise, the remaining children of "src" are broken by the heavy
      // nodes into intervals of consecutive non-heavy nodes:
      std::vector<std::vector<node_type>> ranges;
      for(int i = 0, j; i < children.size(); i = j) {
        int k = i;
        for (;k < children.size() and heavies.count(children[k]); ++k);
        for (j = k; j < children.size() and not heavies.count(children[j]); ++j);
        std::vector<node_type> rng;
        // "children[j]" is a heavy child, hence we do not include it:
        for (;k < j; ++k) {
          rng.push_back(children[k]);
        }
        if (!rng.empty()) {
          ranges.push_back(std::move(rng));
        }
      }

      // We consider each interval separately, treating each as in Case 1:
      std::vector<Component> result;
      for (const auto &r : ranges) {
        const auto ret = caseOne(src, r, temporary, L);
        result.insert(result.end(), ret.begin(), ret.end());
      }

      return result;
    }

   public:

    ~TreeCovering() override = default;

    explicit TreeCovering(std::istream& is) {
      is >> n;
      for(auto i = 0; i < n; ++i) {
        m_adj[i].clear();
      }
      for (int k = 0; k < n-1; ++k) {
        node_type i, j;
        is >> i >> j; // 1-based index
        --i, --j;
        auto idxF = m_manager.add({i,j});
        auto idxB = m_manager.add({j,i});
        m_adj[i].push_back(idxF);
        m_adj[j].push_back(idxB);
      }
      std::fill(m_seen.begin(), m_seen.begin()+n, false);
      std::fill(m_parent.begin(), m_parent.begin()+n, -1);
      calcCard(0);
    }

    void print(std::ostream& os, const size_t L) override {
      m_permanent_components.clear();
      auto res = decompose(0,L);
      if (!res.empty()) {
        m_permanent_components.insert(m_permanent_components.end(),
                                      res.begin(), res.end());
      }
      int component = 0;
      for (const auto &c : m_permanent_components) {
        os << "[Component No] " << ++component << '\n';
        if (c.root) {
          os << "Single-node cluster: " << 1 + *(c.root) << '\n';
          continue;
        }
        for (auto idx : c.edge_idx) {
          const auto x = m_manager.sourceOf(idx);
          const auto y = m_manager.destinationOf(idx);
          os << x+1 << "->" << y+1 << '\n';
        }
      }
    }

  };
} // namespace

std::shared_ptr<ITreeCovering> createTreeCovering(std::istream& is) {
  return std::make_shared<TreeCovering>(is);
}